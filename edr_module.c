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

#define ASSERT_PID(x) current->pid == x

struct ftrace_hook {
    const char *name;
    ftrace_func_t ftrace_callback;
    void *replacement;
    void *original;
    unsigned long address;
    struct ftrace_ops ops;
};

typedef __u32 EDR_BIT;

#define MAX_INDEX_HOOKS 32
#define MAX_INDEX_RULE MAX_INDEX_HOOKS
#define MAX_INDEX_PATH_HOOK MAX_INDEX_HOOKS
#define MAX_INDEX_FNAME_HOOK MAX_INDEX_HOOKS

struct command_group {
    struct command *commands; 
    int command_count;
};

struct edr_rule {
    struct command *commands; 
    uint8_t command_count;

    struct command_group cmd_or;
    struct command_group cmd_and;

    char *paths[MAX_INDEX_RULE];   
    char *fnames[MAX_INDEX_RULE];  

    enum edr_action action;
    uint8_t flag;
};

struct edr_system {
    struct edr_rule *rules[MAX_INDEX_RULE];     
    uint8_t rule_count[MAX_INDEX_RULE];

    EDR_BIT hooked;
};

struct edr_system edr_module;


// Set a hook flag
static inline void edr_set_hook(struct edr_system *sys, EDR_BIT flag) {
    sys->hooked |= flag;
}

// Check if a hook flag is set
static inline bool edr_is_hook(struct edr_system sys, EDR_BIT flag) {
    return (sys.hooked & flag) != 0;
}

static inline bool edr_is_flag(EDR_BIT bit, EDR_BIT flag) {
    return (bit & flag) != 0;
}

static inline EDR_BIT edr_get_hook_bit(const char *hook_name) {
    if (strcmp(hook_name, HOOK_OPEN_STR) == 0)   return HOOK_OPEN_BIT;
    if (strcmp(hook_name, HOOK_READ_STR) == 0)   return HOOK_READ_BIT;
    if (strcmp(hook_name, HOOK_WRITE_STR) == 0)  return HOOK_WRITE_BIT;
    if (strcmp(hook_name, HOOK_UNLINK_STR) == 0) return HOOK_UNLINK_BIT;
    return 0;
}


int edr_get_hook_index(const char *hook_name) {
    if (strcmp(hook_name, HOOK_OPEN_STR) == 0)   return HOOK_OPEN_INDEX;
    if (strcmp(hook_name, HOOK_READ_STR) == 0)   return HOOK_READ_INDEX;
    if (strcmp(hook_name, HOOK_WRITE_STR) == 0)  return HOOK_WRITE_INDEX;
    if (strcmp(hook_name, HOOK_UNLINK_STR) == 0) return HOOK_UNLINK_INDEX;
    return -1; // Unknown
}

#define EDR_RING_SIZE 512

#ifdef EDR_QUEUE_RING_BUFFER 
struct edr_ring_buffer {
    struct edr_event buffer[EDR_RING_SIZE];
    unsigned int head;
    unsigned int tail;
    spinlock_t lock;
    wait_queue_head_t wq;
};
static struct edr_ring_buffer edr_rb;

#endif

static struct sock *nl_sk = NULL;
static u32 pid_recv = 0;
static struct task_struct *edr_thread;

#ifdef EDR_QUEUE_LIST
struct edr_event_queue {
    struct list_head list;
    struct edr_event event;  
};
static LIST_HEAD(edr_event_list);
static DEFINE_MUTEX(edr_event_lock);
static DECLARE_WAIT_QUEUE_HEAD(edr_event_wq);
static atomic_t edr_has_event = ATOMIC_INIT(0);
#endif

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
static void free_edr_module(void);

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


#ifdef EDR_QUEUE_LIST
void edr_queue_event_ll(struct edr_event *sample)
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

static int edr_worker_thread_ll(void *data)
{
    while (!kthread_should_stop()) {
        wait_event_interruptible(edr_event_wq,
                                 atomic_read(&edr_has_event) ||
                                 kthread_should_stop());
        struct edr_event_queue *item = NULL;
        
        mutex_lock(&edr_event_lock);
        if (list_empty(&edr_event_list)) {
            atomic_set(&edr_has_event, 0);
            mutex_unlock(&edr_event_lock);
            continue;
        }

        item = list_first_entry(&edr_event_list, struct edr_event_queue , list);
        list_del(&item->list);
        mutex_unlock(&edr_event_lock);

        send_edr_event(&item->event, pid_recv, 0); 
    
        kfree(item->event.fname);
        kfree(item->event.path);
        kfree(item);         
    }

    pr_info("EDR thread exiting cleanly.\n");
    return 0;
}
#endif


#ifdef EDR_QUEUE_RING_BUFFER
void edr_queue_event_rb(struct edr_event *sample)
{
    struct edr_event ev_local;
    unsigned long flags;
    int next;

    spin_lock_irqsave(&edr_rb.lock, flags);

    next = (edr_rb.head + 1) % EDR_RING_SIZE;
    if (next == edr_rb.tail) {
        spin_unlock_irqrestore(&edr_rb.lock, flags);
        return;
    }

    memset(&ev_local, 0, sizeof(ev_local));
    ev_local = *sample;

    if (sample->fname)
        ev_local.fname = kstrdup(sample->fname, GFP_ATOMIC);
    if (sample->path)
        ev_local.path = kstrdup(sample->path, GFP_ATOMIC);

    edr_rb.buffer[edr_rb.head] = ev_local;
    edr_rb.head = next;

    spin_unlock_irqrestore(&edr_rb.lock, flags);
    wake_up_interruptible(&edr_rb.wq);
}

#define EDR_THREAD_TIMEOUT (HZ) // 1 seccond
static int edr_worker_thread_rb(void *data)
{
    while (!kthread_should_stop()) {
        struct edr_event ev;
        unsigned long flags;
        wait_event_interruptible_timeout(edr_rb.wq,
            edr_rb.head != edr_rb.tail || kthread_should_stop(),
            EDR_THREAD_TIMEOUT);
        
        spin_lock_irqsave(&edr_rb.lock, flags);

        if (edr_rb.head == edr_rb.tail) {
            spin_unlock_irqrestore(&edr_rb.lock, flags);
            continue;
        }
        
        ev = edr_rb.buffer[edr_rb.tail];
        edr_rb.tail = (edr_rb.tail + 1) % EDR_RING_SIZE;
        spin_unlock_irqrestore(&edr_rb.lock, flags);

        send_edr_event(&ev, pid_recv, MSG_DONTWAIT);

        if (ev.fname)
            kfree(ev.fname);
        if (ev.path)
            kfree(ev.path);

    }

    pr_info("EDR thread exiting cleanly.\n");
    return 0;
}

#endif

#ifdef EDR_QUEUE_RING_BUFFER
    #define EDR_QUEUE_EVENT(ev) \
        do { edr_queue_event_rb(ev); } while (0)
#elif defined(EDR_QUEUE_LIST)
    #define EDR_QUEUE_EVENT(ev) \
        do { edr_queue_event_ll(ev); } while (0)
#else
    #error "You must define either EDR_QUEUE_RING_BUFFER or EDR_QUEUE_LIST"
#endif

static void netlink_recv(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    struct edr_event_cmd *cmd;
    struct edr_rule *rule, *new_rules_ptr;
    const char *fname, *path;
    void *data;
    int i, j, index, rule_idx;
    int and_count, or_count;

    nlh = nlmsg_hdr(skb);
    data = nlmsg_data(nlh);
    cmd = (struct edr_event_cmd *)data;

    if (edr_is_flag(cmd->flags, EDR_EVENT_SET)) {
        fname = (char *)cmd + cmd->fname_offset;
        path  = (char *)cmd + cmd->path_offset;

        pr_info("[EDR_CMD] Received SET command for rule ID: %s\n", cmd->id);
        pr_info("[EDR_CMD] Target file: fname='%s', path='%s'\n", fname, path);

        for (i = 0; i < cmd->hooked_count; ++i) {
            if (cmd->hooked[i][0] == '\0') {
                continue;
            }

            index = edr_get_hook_index(cmd->hooked[i]);
            if (index < 0 || index >= MAX_INDEX_RULE) {
                pr_warn("[EDR_CMD] Invalid hook name: %s\n", cmd->hooked[i]);
                continue;
            }
            
            pr_info("[EDR_CMD] Processing hook[%d]: %s (index: %d)\n", i, cmd->hooked[i], index);

            rule_idx = edr_module.rule_count[index];

            new_rules_ptr = krealloc(
                edr_module.rules[index],
                sizeof(struct edr_rule) * (rule_idx + 1),
                GFP_KERNEL
            );

            if (!new_rules_ptr) {
                pr_err("[EDR_CMD] Failed to reallocate memory for rules array (index: %d)\n", index);
                continue; 
            }
            edr_module.rules[index] = new_rules_ptr;

            rule = &edr_module.rules[index][rule_idx];
            memset(rule, 0, sizeof(struct edr_rule));
            

            and_count = 0;
            or_count = 0;
            for (j = 0; j < cmd->command_count; ++j) {
                if (cmd->command[j].flag == COMMAND_AND) {
                    and_count++;
                } else {
                    or_count++;
                }
            }
            
            if (and_count > 0) {
                rule->cmd_and.commands = kmalloc(and_count * sizeof(struct command), GFP_KERNEL);
                if (!rule->cmd_and.commands) {
                    pr_err("[EDR_CMD] Failed to allocate memory for AND commands\n");
                    goto cleanup_rule; 
                }
            }
            if (or_count > 0) {
                rule->cmd_or.commands = kmalloc(or_count * sizeof(struct command), GFP_KERNEL);
                if (!rule->cmd_or.commands) {
                    pr_err("[EDR_CMD] Failed to allocate memory for OR commands\n");
                    goto cleanup_rule; 
                }
            }
            
            rule->command_count = cmd->command_count;
            rule->cmd_and.command_count = 0;
            rule->cmd_or.command_count = 0;
            for (j = 0; j < cmd->command_count; ++j) {
                if (cmd->command[j].flag == COMMAND_AND) {
                    rule->cmd_and.commands[rule->cmd_and.command_count++] = cmd->command[j];
                } else {
                    rule->cmd_or.commands[rule->cmd_or.command_count++] = cmd->command[j];
                }
            }

            rule->paths[index] = kstrdup(path, GFP_KERNEL);
            rule->fnames[index] = kstrdup(fname, GFP_KERNEL);
            if (!rule->paths[index] || !rule->fnames[index]) {
                pr_err("[EDR_CMD] Failed to duplicate path/fname strings\n");
                goto cleanup_rule; 
            }

            rule->action = (cmd->action == EDR_ACTION_BLOCK) ? EDR_ACTION_BLOCK : EDR_ACTION_MONITOR;
            
            if (edr_is_flag(cmd->flags, EDR_EVENT_CHECK_PATH)) {
                rule->flag |= EDR_EVENT_CHECK_PATH;
            }

            edr_module.hooked |= (1ULL << index);
            edr_module.rule_count[index]++;
                        
            continue; 

        cleanup_rule:
            pr_err("[EDR_CMD] Cleaning up failed rule for hook index %d\n", index);
            if (rule->cmd_and.commands)
                kfree(rule->cmd_and.commands);

            if (rule->cmd_or.commands)
                kfree(rule->cmd_or.commands);

            if (rule->paths && rule->paths[index])
                kfree(rule->paths[index]);

            if (rule->fnames && rule->fnames[index])
                kfree(rule->fnames[index]);

            memset(rule, 0, sizeof(struct edr_rule));
        }
    } 
    /* --- Xử lý lệnh CLEAR (xóa tất cả rules) --- */
    else if (edr_is_flag(cmd->flags, EDR_EVENT_CLEAR)) {
        pr_info("[EDR_CMD] Received CLEAR command. Freeing all rules.\n");
        free_edr_module();
    }
}

char *get_path_fsf(struct path *ppath)
{
    char *page_buf, *path, *out;
    size_t len;

    page_buf = (char *)__get_free_page(GFP_KERNEL);
    if (!page_buf)
        return NULL;

    path = d_path(ppath, page_buf, PAGE_SIZE);
    if (IS_ERR(path)) {
        free_page((unsigned long)page_buf);
        return NULL;
    }

    len = strlen(path) + 1;

    out = kmalloc(len, GFP_KERNEL);
    if (!out) {
        free_page((unsigned long)page_buf);
        return NULL;
    }
    memcpy(out, path, len);
    free_page((unsigned long)page_buf);

    return out;
}


static bool match_command(const struct command *cmd, const char *ev_comm, const char *ev_name, const char *ev_path, const char * edr_str) {
    const char *val = NULL;
    if (strcmp(cmd->field, "comm") == 0) val = ev_comm;
    else if (strcmp(cmd->field, "name") == 0) val = ev_name;
    else if (strcmp(cmd->field, "path") == 0) val = ev_path;
    else if (strcmp(cmd->field, "syscall") == 0) val = edr_str;
    else return false;

    if (cmd->operator == OPERATOR_EQUALS)
        return strcmp(cmd->value, val) == 0;
    return strstr(cmd->value, val) != NULL;
}

int edr_check_hook(struct edr_event *event, EDR_BIT flag, const char *edr_str)
{
    int r, c;
    int rule_count;
    int index;
    const char *ev_path;
    const char *ev_comm;
    const char *ev_name;
    struct edr_rule *rule;
    bool match_and;
    bool match_or;

    if (!event || !edr_str)
        return 0;

    ev_path = event->path ? event->path : "";
    ev_comm = event->comm ? event->comm : "";
    ev_name = event->fname ? event->fname : "";

    index = edr_get_hook_index(edr_str);
    if (index < 0 || !edr_is_hook(edr_module, flag))
        return 0;

    rule_count = edr_module.rule_count[index];

    for (r = 0; r < rule_count; ++r) {
        rule = &edr_module.rules[index][r];

        /* check path */
        if (!rule->paths || !rule->paths[index])
            continue;

        /* check EDR_EVENT_CHECK_PATH */
        if (edr_is_flag(rule->flag, EDR_EVENT_CHECK_PATH)) {
            if (!strstr(ev_path, rule->paths[index]))
                continue;
        } else {
            if (strcmp(ev_path, rule->paths[index]) != 0)
                continue;
        }

        /* check AND command */
        match_and = true;
        for (c = 0; c < rule->cmd_and.command_count; ++c) {
            if (!match_command(&rule->cmd_and.commands[c], ev_comm, ev_name, ev_path, edr_str)) {
                match_and = false;
                break;
            }
        }

        /* check OR command */
        match_or = false;
        for (c = 0; c < rule->cmd_or.command_count; ++c) {
            if (match_command(&rule->cmd_or.commands[c], ev_comm, ev_name, ev_path, edr_str)) {
                match_or = true;
                break;
            }
        }

        if (match_and || match_or) {
            pr_info("[EDR] Rule matched: comm = %s, syscall = %s\n", ev_comm, edr_str);
            if (rule->action == EDR_ACTION_BLOCK) {
                pr_info("[EDR] Action = BLOCK. Blocking operation.\n");
            } else {
                pr_info("[EDR] Action = MONITOR. Logging only.\n");
            }
        }
    }

    return 0;
}

static asmlinkage long file_hooked_open(int dfd, const char __user *filename, int flags, umode_t mode)
{
    struct path path;
    struct edr_event event;
    char * path_buffer;
    int error;

    memset(&event, 0, sizeof(struct edr_event));

    setup_event(event, "file_open");

    if(ASSERT_PID(pid_recv)) {
        goto out;
    }

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

    event.path = d_path(&path, path_buffer, PATH_MAX);
    if (IS_ERR(event.path)) {
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
    
    // if(strstr(event.path, "socket_netlink")){
    //     pr_info("[EDR] Path matched: path == %s\n", event.path);
    //     pr_info("[EDR] Path matched: name == %s\n", event.fname);
    //     pr_info("[EDR] Path matched: comm == %s\n", event.comm);
    // }

    error = edr_check_hook(&event, HOOK_OPEN_BIT, HOOK_OPEN_STR);
    if(error < 0){
        return error;
    }

    EDR_QUEUE_EVENT(&event);

    if (path.dentry)
        path_put(&path);

out:
    return file_original_open(dfd, filename, flags, mode);
}


static asmlinkage ssize_t file_hooked_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct edr_event event;
    int err;

    memset(&event, 0, sizeof(struct edr_event));

    if(ASSERT_PID(pid_recv)) {
        goto out;
    }

    setup_event(event, "file_read");

    if(file) {
        event.path = get_path_fsf(&file->f_path);

        event.fname = kstrdup(file->f_path.dentry->d_name.name, GFP_KERNEL);
        if (!event.fname) {
            pr_err("Failed to allocate memory for event.fname\n");
            if (event.path)
                kfree(event.path);
            goto out;
        }

        err = edr_check_hook(&event, HOOK_READ_BIT, HOOK_READ_STR);
        if(err < 0){
            return err;
        }

        EDR_QUEUE_EVENT(&event);
    }

out:
    return file_original_read(file, buf, count, pos);
}

static asmlinkage ssize_t file_hooked_write(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    struct edr_event event;
    int err;
    
    memset(&event, 0, sizeof(struct edr_event));

    if(ASSERT_PID(pid_recv)) {
        goto out;
    }
    setup_event(event, "file_write");

    if(file) {
        event.path = get_path_fsf(&file->f_path);

        event.fname = kstrdup(file->f_path.dentry->d_name.name, GFP_KERNEL);
        if (!event.fname) {
            pr_err("Failed to allocate memory for event.fname\n");
            if (event.path)
                kfree(event.path);
            goto out;
        }

        err = edr_check_hook(&event, HOOK_WRITE_BIT, HOOK_WRITE_STR);
        if(err < 0){
            return err;
        }

        EDR_QUEUE_EVENT(&event);
    }
out:
    return file_original_write(file, buf, count, pos);
}

static asmlinkage int file_hooked_unlink(struct user_namespace *mnt_userns, struct inode *dir,
                                         struct dentry *dentry, struct inode **delegated_inode) {
    char *path_buffer = NULL;
    const char *file_name;
    struct edr_event event;
    int err;

    memset(&event, 0, sizeof(struct edr_event));
    setup_event(event, "unlink");

    file_name = dentry->d_name.name;

    path_buffer = kmalloc(PATH_MAX, GFP_KERNEL);
    if (!path_buffer) {
        pr_err("Failed to allocate memory for path_buffer\n");
        goto out;
    }

    event.path = dentry_path_raw(dentry, path_buffer, PATH_MAX);
    if (IS_ERR(event.path)) {  
        pr_err("Failed to resolve full path for file: %s\n", file_name);
        kfree(path_buffer);
        goto out;
    }

    event.fname = kstrdup(file_name, GFP_KERNEL);
    if (!event.fname) {
        pr_err("Failed to allocate memory for fname in event\n");
        kfree(path_buffer);
        goto out;
    }

    err = edr_check_hook(&event, HOOK_UNLINK_BIT, HOOK_UNLINK_STR);
    if (err < 0) {
        pr_err("Hook check failed with error: %d\n", err);
        kfree(event.fname);
        kfree(path_buffer);
        return err;
    }

    EDR_QUEUE_EVENT(&event);

    kfree(path_buffer); 

out:
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

#ifdef EDR_QUEUE_RING_BUFFER 
    init_waitqueue_head(&edr_rb.wq);
#endif

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

static void free_edr_module(void)
{
    size_t i, j, k;
    struct edr_rule *rule;

    for (i = 0; i < MAX_INDEX_RULE; ++i) {
        if (!edr_module.rules[i])
            continue;

        for (j = 0; j < edr_module.rule_count[i]; ++j) {
            rule = &edr_module.rules[i][j];

            /* Free all paths */
            if (rule->paths) {
                for (k = 0; k < MAX_INDEX_PATH_HOOK; ++k) {
                    kfree(rule->paths[k]);
                    rule->paths[k] = NULL;
                }
            }

            /* Free all fnames */
            if (rule->fnames) {
                for (k = 0; k < MAX_INDEX_FNAME_HOOK; ++k) {
                    kfree(rule->fnames[k]);
                    rule->fnames[k] = NULL;
                }
            }

            /* Free command groups */
            if (rule->commands) {
                kfree(rule->commands);
                rule->commands = NULL;
            }

            if (rule->cmd_and.commands) {
                kfree(rule->cmd_and.commands);
                rule->cmd_and.commands = NULL;
            }

            if (rule->cmd_or.commands) {
                kfree(rule->cmd_or.commands);
                rule->cmd_or.commands = NULL;
            }

            /* Reset counters and flags */
            rule->command_count = 0;
            rule->cmd_and.command_count = 0;
            rule->cmd_or.command_count = 0;
            rule->flag = 0;
        }

        /* Free rule array */
        kfree(edr_module.rules[i]);
        edr_module.rules[i] = NULL;
        edr_module.rule_count[i] = 0;
    }

    edr_module.hooked = 0;
}

static void __exit ftrace_hook_exit(void)
{
    int i;

    free_edr_module();

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
