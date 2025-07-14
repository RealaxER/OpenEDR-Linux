// SPDX-License-Identifier: GPL-2.0

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ftrace.h>
#include <linux/version.h>
#include <linux/kprobes.h>
#include "edr_ftrace.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("buihien29112002@gmail.com");
MODULE_DESCRIPTION("Module edr use ftrace for block and monitor");
MODULE_VERSION("0.1");

struct ftrace_hook {
    const char *name;
    ftrace_func_t ftrace_callback;
    void *replacement;
    void *original;
    unsigned long address;
    struct ftrace_ops ops;
};

#define HOOK_COUNT (sizeof(hooks) / sizeof(hooks[0]))

static unsigned long (*_kallsyms_lookup_name)(const char *name) = NULL;

static void notrace common_dispatcher(unsigned long ip, unsigned long parent_ip,
                                      struct ftrace_ops *ops, struct ftrace_regs *fregs)
{
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

    if (!within_module(parent_ip, THIS_MODULE)) {
        set_function_return(&fregs->regs, hook->replacement);
    }
}

/*function ogirinal*/
static asmlinkage long (*file_original_open)(int dfd, const char __user *filename,
                                           int flags, umode_t mode);
static asmlinkage ssize_t (*file_original_read)(struct file *, char __user *, size_t, loff_t *);
static asmlinkage ssize_t (*file_original_write)(struct file *, const char __user *, size_t, loff_t *);
static asmlinkage int (*file_original_unlink)(struct user_namespace *mnt_userns, struct inode *dir,
                struct dentry *dentry, struct inode **delegated_inode);
static asmlinkage int (*file_original_bprm_execve)(struct linux_binprm *bprm, int fd, struct filename *filename, int flags);


static asmlinkage long file_hooked_open(int dfd, const char __user *filename, int flags, umode_t mode) {
    char fname_buf[256];

    if (copy_from_user(fname_buf, filename, sizeof(fname_buf) - 1) == 0) {
            if (strstr(fname_buf, "f_open.txt") != NULL) {
                pr_info ("FILE BLOCKED %s (pid=%d, tgid=%d) tried to open %s\n",
                       current->comm, current->pid, current->tgid, fname_buf);
                return -EPERM; 
            }
    }else {
        pr_err("Failed to copy user filename when hook do_sys_open");
    }

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

    return 0;
}


static void remove_hook(struct ftrace_hook *hook) {
    ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
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
    // {
    //     .name = "vfs_unlink",
    //     .replacement = file_hooked_unlink,
    //     .original = &file_original_unlink,
    //     .ftrace_callback = common_dispatcher,
    // },
    // {
    //     .name = "bprm_execve",
    //     .replacement = file_hooked_bprm_execve,
    //     .original = &file_original_bprm_execve,
    //     .ftrace_callback = common_dispatcher,
    // },
};

/*function init*/
static int __init ftrace_hook_init(void){
    int ret;
    unsigned long flags = FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_IPMODIFY;
    size_t i;
    ret = get_func_kallsyms_lookup_name();

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
    while (i--) remove_hook(&hooks[i]);
    return ret;
}

static void __exit ftrace_hook_exit(void)
{
    size_t i;
    for (i = 0; i < HOOK_COUNT; i++) {
        remove_hook(&hooks[i]);
        pr_info("Removed hook for %s\n", hooks[i].name);
    }
}

module_init(ftrace_hook_init);
module_exit(ftrace_hook_exit);
