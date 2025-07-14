// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ftrace.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/kprobes.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/slab.h>       
#include <linux/fs.h>         
#include <linux/dcache.h>  
#include <linux/namei.h>     
#include <linux/limits.h>    
#include <linux/string.h> 
#include <linux/ktime.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/list.h>

#include "edr_module.h"
#include "netlink.h"


struct ftrace_hook {
    const char *name;
    ftrace_func_t ftrace_callback;
    void *replacement;
    void *original;
    unsigned long address;
    struct ftrace_ops ops;
};

struct edr_event_queue {
    struct list_head list;
    struct edr_event event;  // dữ liệu bạn đã tạo
};

static struct sock *nl_sk = NULL;
static u32 pid_recv = 0;

static LIST_HEAD(edr_event_list);
static DEFINE_MUTEX(edr_event_lock);
static DECLARE_WAIT_QUEUE_HEAD(edr_event_wq);
static struct task_struct *edr_thread;
static atomic_t edr_has_event = ATOMIC_INIT(0);

static int module_unloading;

#define HOOK_COUNT (sizeof(hooks) / sizeof(hooks[0]))
static unsigned long (*_kallsyms_lookup_name)(const char *name) = NULL;

/*function ogirinal*/
static asmlinkage long (*file_original_open)(int dfd, const char __user *filename,
                                           int flags, umode_t mode);

static asmlinkage ssize_t (*file_original_read)(struct file *, char __user *, size_t, loff_t *);

static asmlinkage ssize_t (*file_original_write)(struct file *, const char __user *, size_t, loff_t *);

static asmlinkage int (*file_original_unlink)(struct user_namespace *mnt_userns, struct inode *dir,
	       struct dentry *dentry, struct inode **delegated_inode);

static asmlinkage int (*file_original_execvet) (int fd, struct filename *filename,
		const char __user *const __user *__argv,
		const char __user *const __user *__envp,
		int flags);

/*function handle ftrace*/
static int resolve_hook_address(struct ftrace_hook *hook) {
    hook->address = _kallsyms_lookup_name(hook->name);
    if (!hook->address) {
        pr_err("Cannot resolve %s\n", hook->name);
        return -ENOENT;
    }
    *((unsigned long *)hook->original) = hook->address;
    return 0;
}

static void notrace common_dispatcher(unsigned long ip, unsigned long parent_ip,
                                      struct ftrace_ops *ops, struct ftrace_regs *fregs)
{
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

    if (unlikely(READ_ONCE(module_unloading))) {
        return;
    }
    
    if (!within_module(parent_ip, THIS_MODULE)) {
        set_function_ip(&fregs->regs, hook->replacement);
    }
}

long strncpy_from_user_nofault(char *dst, const void __user *unsafe_addr,
			      long count)
{
	long ret;

	if (unlikely(count <= 0))
		return 0;

	pagefault_disable();
	ret = strncpy_from_user(dst, unsafe_addr, count);
	pagefault_enable();

	if (ret >= count) {
		ret = count;
		dst[ret - 1] = '\0';
	} else if (ret >= 0) {
		ret++;
	}

	return ret;
}


static void send_edr_event(struct edr_event *sample, int user_pid, int flags)
{
    struct nlmsghdr *nlh = NULL;
    struct sk_buff *skb = NULL;
    struct edr_event_hdr *hdr = NULL;

    size_t fname_len = strlen(sample->fname) + 1;
    size_t path_len = strlen(sample->path) + 1;

    size_t total_size = sizeof(struct edr_event_hdr) + fname_len + path_len;

    char *buffer = kmalloc(total_size, GFP_KERNEL);
    if (!buffer) {
        pr_warn("kmalloc failed when send msg\n");
        return;
    }

    hdr = (struct edr_event_hdr *)buffer;

    strncpy(hdr->event, sample->event, TASK_COMM_LEN);
    hdr->pid            = sample->pid;
    hdr->ppid           = sample->ppid;
    hdr->tgid           = sample->tgid;
    hdr->uid            = sample->uid;
    hdr->flags          = sample->flags;
    hdr->result         = sample->result;
    hdr->timestamp_ns   = sample->timestamp_ns;
    hdr->total_size     = total_size;
    strncpy(hdr->comm, sample->comm, TASK_COMM_LEN);

    hdr->fname_offset = sizeof(*hdr);
    strcpy(buffer + hdr->fname_offset, sample->fname);

    hdr->path_offset = hdr->fname_offset + fname_len;
    strcpy(buffer + hdr->path_offset, sample->path);

    skb = nlmsg_new(total_size, GFP_KERNEL);
    if (!skb) {
        pr_warn("EDR: nlmsg_new failed\n");
        kfree(buffer);
        return;
    }

    nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, total_size, 0);
    memcpy(nlmsg_data(nlh), buffer, total_size);
    kfree(buffer);

    netlink_unicast(nl_sk, skb, user_pid, flags);
}


void edr_queue_event(struct edr_event *sample)
{
    struct edr_event_queue *item = kmalloc(sizeof(*item), GFP_KERNEL);
    if (!item) return;

    memcpy(&item->event, sample, sizeof(struct edr_event));

    mutex_lock(&edr_event_lock);
    list_add_tail(&item->list, &edr_event_list);
    atomic_set(&edr_has_event, 1);
    mutex_unlock(&edr_event_lock);

    wake_up_interruptible(&edr_event_wq);
}

static int edr_worker_thread(void *data)
{
    while (!kthread_should_stop()) {
        wait_event_interruptible(edr_event_wq,
                                 atomic_read(&edr_has_event) ||
                                 kthread_should_stop());

        while (1) {
            struct edr_event_queue *item = NULL;

            mutex_lock(&edr_event_lock);
            if (list_empty(&edr_event_list)) {
                atomic_set(&edr_has_event, 0);
                mutex_unlock(&edr_event_lock);
                break;
            }

            item = list_first_entry(&edr_event_list, struct edr_event_queue , list);
            list_del(&item->list);
            mutex_unlock(&edr_event_lock);

            send_edr_event(&item->event, pid_recv, 0); 
        
            kfree(item->event.fname);
            kfree(item->event.path);
            kfree(item);         
        }
    }
    return 0;
}

static void netlink_recv(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    struct edr_netlink_cmd *msg;

    nlh = nlmsg_hdr(skb);
    msg = (struct edr_netlink_cmd *)nlmsg_data(nlh);

    pr_info("netlink command: %s for PID: %d\n", msg->cmd, msg->pid);

    if (strncmp(msg->cmd, "start", 5) == 0) {
        pid_recv = nlh->nlmsg_pid;

        if (!edr_thread) {
            edr_thread = kthread_run(edr_worker_thread, NULL, "edr_sender");
            if (IS_ERR(edr_thread)) {
                pr_err("Failed to create worker thread: %ld\n", PTR_ERR(edr_thread));
                edr_thread = NULL;
            } else {
                pr_info("EDR worker thread started by PID %d\n", pid_recv);
            }
        } else {
            pr_info("EDR worker thread already running.\n");
        }

    } else if (strncmp(msg->cmd, "stop", 4) == 0) {
        if (edr_thread) {
            wake_up_interruptible(&edr_event_wq);
            kthread_stop(edr_thread);
            edr_thread = NULL;
            pr_info("EDR worker thread stopped by PID %d\n", pid_recv);
        } else {
            pr_info("No running EDR thread to stop.\n");
        }

    } else {
        pr_warn("Unknown command received from user: %s\n", msg->cmd);
    }
}


static asmlinkage long file_hooked_open(int dfd, const char __user *filename, int flags, umode_t mode)
{
    struct path path;
    struct edr_event event;
    char * path_buffer;
    char * path_str = NULL;
    int error;

    memset(&event, 0, sizeof(struct edr_event));

    setup_event(event, "open");

    if (!filename) {
        goto out;
    }

    error = user_path_at(dfd, filename, LOOKUP_FOLLOW, &path);
    if (error) {
        goto out;
    }

    path_buffer = kmalloc(PATH_MAX, GFP_KERNEL);
    if (!path_buffer) {
        path_put(&path);
        goto out;
    }

    path_str = d_path(&path, path_buffer, PATH_MAX);
    if (IS_ERR(path_str)) {
        kfree(path_buffer);
        path_put(&path);
        goto out;
    }

    event.fname = kstrdup(path.dentry->d_name.name, GFP_KERNEL);
    if (!event.fname) {
        pr_err("Failed to allocate memory for event.fname\n");
        kfree(path_buffer);
        path_put(&path);
        goto out;
    }

    
    event.path = kstrdup(path_str, GFP_KERNEL);
    if (!event.fname) {
        pr_err("Failed to allocate memory for event.fname\n");
        kfree(path_buffer);
        kfree(event.fname);
        path_put(&path);
        goto out;
    }

    edr_queue_event(&event);
    
    if (path_buffer)
        kfree(path_buffer);

    if (path.dentry)
        path_put(&path);

out:
    return file_original_open(dfd, filename, flags, mode);
}


static asmlinkage ssize_t file_hooked_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    if(file->f_path.dentry->d_name.name){
        if (strstr(file->f_path.dentry->d_name.name, "f_read.txt") != NULL) {
            pr_info ("FILE BLOCKED %s (pid=%d, tgid=%d) tried to read %s\n",
            current->comm, current->pid, current->tgid, file->f_path.dentry->d_name.name);
                return -EPERM; 
        }
    }
    return file_original_read(file, buf, count, pos);
}

static asmlinkage ssize_t file_hooked_write(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    if(file->f_path.dentry->d_name.name){
        if (strstr(file->f_path.dentry->d_name.name, "f_write.txt") != NULL) {
            pr_info ("FILE BLOCKED %s (pid=%d, tgid=%d) tried to write %s\n",
            current->comm, current->pid, current->tgid, file->f_path.dentry->d_name.name);
                return -EPERM; 
        }
    }
    return file_original_write(file, buf, count, pos);
}

static asmlinkage int file_hooked_unlink(struct user_namespace *mnt_userns, struct inode *dir,
	       struct dentry *dentry, struct inode **delegated_inode) {

    return file_original_unlink(mnt_userns, dir, dentry, delegated_inode);
}

static asmlinkage int file_hooked_execvet (int fd, struct filename *filename,
		const char __user *const __user *__argv,
		const char __user *const __user *__envp,
		int flags) {

    return file_original_execvet(fd, filename, __argv, __envp, flags);
}

static int register_hook(struct ftrace_hook *hook, long unsigned int flags) {
    int err;

    err = resolve_hook_address(hook);
    if (err)
        return err;

    if (!hook->replacement) {
        pr_err("replacement function is NULL for hook %s\n", hook->name);
        return -EINVAL;
    }

    if (!hook->original) {
        pr_err("original pointer is NULL for hook %s\n", hook->name);
        return -EINVAL;
    }

    if (!hook->ftrace_callback) {
        pr_err("ftrace_callback (dispatcher) is NULL for hook %s\n", hook->name);
        return -EINVAL;
    }

    hook->ops.func = hook->ftrace_callback;
    hook->ops.flags = flags;

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if (err) {
        pr_err("ftrace_set_filter_ip failed: %d\n", err);
        return err;
    }

    err = register_ftrace_function(&hook->ops);
    if (err) {
        pr_err("register_ftrace_function failed: %d\n", err);
        ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
        return err;
    }

    pr_info("Hook Name: %s, Address: %p, Ops Func: %p\n",
           hook->name, (void *)hook->address, (void *)hook->ops.func);
    return 0;
}

static void remove_hook(struct ftrace_hook *hook) {
    int ret = 0;
    unsigned long address = _kallsyms_lookup_name(hook->name);

    if (address != hook->address) {
        pr_warn("Address mismatch for %s: recorded=0x%lx, current=0x%lx\n",
                hook->name, hook->address, address);
    }

    ret = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if (ret) {
        pr_err("Failed to remove filter for %s: %d\n", hook->name, ret);
    }
    synchronize_rcu();

    unregister_ftrace_function(&hook->ops);
}

static int get_func_kallsyms_lookup_name(void) {
    struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name",
    };
    int ret;

    ret = register_kprobe(&kp);
    if (ret < 0) {
        pr_err("Failed to register kprobe to find kallsyms_lookup_name: %d\n", ret);
        return ret;
    }

    _kallsyms_lookup_name = (void *)kp.addr;
    unregister_kprobe(&kp);

    if (!_kallsyms_lookup_name) {
        pr_err("Failed to get address of kallsyms_lookup_name\n");
        return -ENOENT;
    }

    pr_info("Resolved kallsyms_lookup_name at %px\n", _kallsyms_lookup_name);
    return 0;
}

static struct ftrace_hook hooks[] = {
    {
        .name = "do_sys_open",
        .replacement = file_hooked_open,
        .original = &file_original_open,
        .ftrace_callback = common_dispatcher,
    },
    {
        .name = "vfs_read",
        .replacement = file_hooked_read,
        .original = &file_original_read,
        .ftrace_callback = common_dispatcher,
    },
    {
        .name = "vfs_write",
        .replacement = file_hooked_write,
        .original = &file_original_write,
        .ftrace_callback = common_dispatcher,
    },
    {
        .name = "vfs_unlink",
        .replacement = file_hooked_unlink,
        .original = &file_original_unlink,
        .ftrace_callback = common_dispatcher,
    },
    {
        .name = "__x64_sys_execve",
        .replacement = file_hooked_execvet,
        .original = &file_original_execvet,
        .ftrace_callback = common_dispatcher,
    },
};

struct netlink_kernel_cfg cfg = {
    .input = netlink_recv,
};

/*function init*/
static int __init ftrace_hook_init(void){
    int ret;
    unsigned long flags = FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_IPMODIFY;
    size_t i;
    ret = get_func_kallsyms_lookup_name();

    nl_sk = netlink_kernel_create(&init_net, NETLINK_EDR, &cfg);
    if (!nl_sk) {
        pr_err("EDR: Cannot create netlink socket\n");
        return -ENOMEM;
    }
    
    if (ret) {
        pr_err("Failed to get addr func kallsyms_lookup_name: %d\n", ret);
        return ret;
    }

    for (i = 0; i < HOOK_COUNT; i++) {
        ret = register_hook(&hooks[i], flags);
        if (ret) {
            pr_err("Failed to register hook for %s\n", hooks[i].name);
            goto cleanup;
        }
    }

    pr_info("All hooks registered successfully\n");
    return 0;

cleanup:
    netlink_kernel_release(nl_sk);

    while (i--) remove_hook(&hooks[i]);

    return ret;
}

static void __exit ftrace_hook_exit(void)
{
    size_t i;
    WRITE_ONCE(module_unloading, 1);
    netlink_kernel_release(nl_sk);

    synchronize_rcu();

    for (i = 0; i < HOOK_COUNT; i++) {
        pr_info("Removing hook for %s\n", hooks[i].name);
        remove_hook(&hooks[i]);
    }
}

module_init(ftrace_hook_init);
module_exit(ftrace_hook_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("buihien29112002@gmail.com");
MODULE_DESCRIPTION("Module edr use ftrace for block and monitor");
MODULE_VERSION("0.1");
