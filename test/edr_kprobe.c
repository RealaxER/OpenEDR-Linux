#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include <linux/version.h>
#include "edr_kprobe.h"
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include "edr_ioctl.h"
#include <linux/net.h>
#include <linux/inet.h>
#include <linux/socket.h>
#include <net/tcp.h>
#include <linux/slab.h>  // kmalloc, kfree
#include <linux/ktime.h>
#include <linux/namei.h>  
#include <linux/pid.h>

#define MAX_FILENAME_LEN 64

#define DEVICE_NAME "edrdev"
#define CLASS_NAME "edr"
#define BLOCK_FILENAME   "f_test.txt"

static dev_t dev_number;
static struct class *edr_class;
static struct cdev edr_cdev;

struct net_event {
    __u32 pid;
    __u32 len;
    __u64 timestamp_ns;
};

struct event_node {
    struct net_event evt;
    struct list_head list;
};


static DECLARE_WAIT_QUEUE_HEAD(event_wait);
static LIST_HEAD(event_list);
static DEFINE_SPINLOCK(event_lock);

char *get_path_fsf(struct path *ppath)
{
    char *buf, *path;
    
    buf = kmalloc(PATH_MAX, GFP_KERNEL);
    if (!buf)
        return NULL;

    path = d_path(ppath, buf, PATH_MAX);
    if (IS_ERR(path)) {
        kfree(buf);
        return NULL;
    }

    if (path != buf)
        memmove(buf, path, strlen(path) + 1);

    return buf;
}

/* Handler before function do_sys_open call */
static int __kprobes file_open_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    pid_t pid = current->pid;
    pid_t tgid = current->tgid;
    const char *comm = current->comm;
    char __user * user_filename;
    char filename[128];

    syscall_get_arguments(current, regs, args);
    user_filename = (char __user *)args[1];

    if (user_filename) {
        if (copy_from_user(filename, user_filename, MAX_FILENAME_LEN - 1) == 0) {
            if (strstr(filename, BLOCK_FILENAME) != NULL) {
                printk(KERN_INFO "[EDR] FILE: BLOCKED %s (pid=%d, tgid=%d) tried to open %s\n",
                       comm, pid, tgid, filename);

                set_syscall_return(regs, -EPERM);
                override_function_with_return(regs);
                return 1;
            }
        }
    }

    return 0;
}

static int __kprobes file_write_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    const char *comm = current->comm;
    struct file *file;
    char *path;
    syscall_get_arguments(current, regs, args);
    
    file = (struct file *)args[0];
    
    if(!file){
        printk(KERN_ERR "FILE: Failed to convert regs to file");
        return 0;
    }
    path = get_path_fsf(&file->f_path);

    if(file->f_path.dentry->d_name.name){
        //printk(KERN_INFO "FILE: name: %s", file->f_path.dentry->d_name.name);
    }

    kfree(path);
    return 0;
}

static int __kprobes file_read_handler(struct kprobe *p, struct pt_regs *regs)
{
    return 0;
}

static int __kprobes file_rename_handler(struct kprobe *p, struct pt_regs *regs)
{  
    unsigned long args[6];
    struct renamedata *rd;
    struct inode *old_dir, *new_dir;
    struct dentry *old_dentry; 
    struct dentry *new_dentry; 

    syscall_get_arguments(current, regs, args);

    rd          = (struct renamedata *)args[0];
    old_dir     = rd->old_dir;
    new_dir     = rd->new_dir;
	old_dentry  = rd->old_dentry;
	new_dentry  = rd->new_dentry;

    if(old_dentry->d_name.name){
        if(strstr(old_dentry->d_name.name, BLOCK_FILENAME) != NULL){
            printk(KERN_INFO "[EDR] FILE: BLOCKED %s (pid=%d, tgid=%d) tried to rename %s\n",
                    current->comm, current->pid, current->tgid, old_dentry->d_name.name);
            set_syscall_return(regs, -EPERM);
            override_function_with_return(regs);
            return 1;
        }
    }

    return 0;
}

static int __kprobes file_unlink_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    struct dentry *dentry;
    syscall_get_arguments(current, regs, args);

    dentry = (struct dentry *)args[2];
    
    if(dentry->d_name.name){
        if(strstr(dentry->d_name.name, BLOCK_FILENAME) != NULL){
            printk(KERN_INFO "[EDR] FILE: BLOCKED %s (pid=%d, tgid=%d) tried to delete %s\n",
                    current->comm, current->pid, current->tgid, dentry->d_name.name);
            set_syscall_return(regs, -EPERM);
            override_function_with_return(regs);
            return 1;
        }
    }

    return 0;
}


static int __kprobes file_execve_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    struct filename *filename;
    syscall_get_arguments(current, regs, args);

    filename = (struct filename *)args[2];
    
    if(filename->name){
        if(strstr(filename->name, "f_test") != NULL){
            printk(KERN_INFO "[EDR] FILE: BLOCKED %s (pid=%d, tgid=%d) tried to execve %s\n",
                    current->comm, current->pid, current->tgid, filename->name);
            set_syscall_return(regs, -EPERM);
            override_function_with_return(regs);
            return 1;
        }
    }

    return 0;
}

static int __kprobes kill_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    struct pid *target_struct;
    struct kernel_siginfo *info;
    pid_t target;
    pid_t sender;
    int sig;
    syscall_get_arguments(current, regs, args);
    target_struct = (struct pid*)args[2];
    info = (struct kernel_siginfo*)args[1];
    sig = (int)args[0];
    if(target_struct && info) {
        if(sig == SIGTERM){
            target = pid_nr(target_struct);
            sender = info->si_pid;
            printk(KERN_INFO "[EDR] PROCESS: Signal kill: %d -> %d (sig=%d)\n", sender, target, sig);
        }
    }
    
    return 0;
}


static int __kprobes module_delete_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    size_t i;
    struct module *mod;

    syscall_get_arguments(current, regs, args);
    mod = (struct module *)args[0];

    if (!mod) {
        printk(KERN_ERR "[EDR] MODULE: Failed to get module for delete");
        return 0;
    }
    // printk(KERN_INFO "[EDR] MODULE removed: name=%s, ver=%s, gpl=%d, sig=%d, taints=%lx, refcnt=%d",
    //     mod->name,
    //     mod->version ? mod->version : "unknown",
    //     mod->using_gplonly_symbols,
    //     mod->sig_ok,
    //     mod->taints,
    //     atomic_read(&mod->refcnt));

    if(strstr(mod->name, "test") != NULL){
        set_syscall_return(regs, -EPERM);
        printk(KERN_INFO "[EDR] MODULE: BLOCKED module %s", mod->name);

        override_function_with_return(regs);
        return 1;
    }

    return 0;
}


static int __kprobes module_find_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    size_t i;
    char * name;

    syscall_get_arguments(current, regs, args);
    name = (char *)args[0];
    if(!name){
        printk(KERN_ERR "[EDR] MODULE: Failed to find module");
        return 0;
    }

    printk(KERN_INFO "[EDR] MODULE finded: %s", name);

    return 0;
}

static int __kprobes module_init_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    size_t i;
    struct module *mod;

    syscall_get_arguments(current, regs, args);
    mod = (struct module *)args[0];

    if (!mod) {
        printk(KERN_ERR "[EDR] MODULE: Failed to get module for init");
        return 0;
    }

    printk(KERN_INFO "[EDR] MODULE init: name=%s, ver=%s, gpl=%d, sig=%d, taints=%lx, refcnt=%d",
        mod->name,
        mod->version ? mod->version : "unknown",
        mod->using_gplonly_symbols,
        mod->sig_ok,
        mod->taints,
        atomic_read(&mod->refcnt));


    return 0;
}

static long edr_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct edr_data data;
    struct edr_data array[EDR_MAX_ARRAY];
    int i;
    switch (cmd) {
        case EDR_IOCTL_SEND_DATA:
            if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
                return -EFAULT;

            printk(KERN_INFO "EDR: Received id= %d, name= %s\n", data.id, data.name);
            break;
        case EDR_IOCTL_SEND_ARRAY:
            if (copy_from_user(array, (void __user *)arg, sizeof(array)))
                return -EFAULT;

            for (i = 0; i < EDR_MAX_ARRAY; i++) {
                printk(KERN_INFO "EDR[%d]: id= %d, name= %s\n", i, array[i].id, array[i].name);
            }
            break;
        default:
            return -ENOTTY;
    }

    return 0;
}

static ssize_t edr_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    struct event_node *node;
    struct net_event evt;

    if (list_empty(&event_list)) {
        if (f->f_flags & O_NONBLOCK)
            return -EAGAIN;
        if (wait_event_interruptible(event_wait, !list_empty(&event_list)))
            return -ERESTARTSYS;
    }

    spin_lock(&event_lock);

    if (list_empty(&event_list)) {
        spin_unlock(&event_lock);
        return 0;
    }
    node = list_first_entry(&event_list, struct event_node, list);
    list_del(&node->list);

    spin_unlock(&event_lock);

    evt = node->evt;

    kfree(node);

    if (len < sizeof(evt))
        return -EINVAL;

    if (copy_to_user(buf, &evt, sizeof(evt)))
        return -EFAULT;

    return sizeof(evt);
}

static int __kprobes tcp_connect_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    struct sockaddr *uaddr;
    struct sockaddr_in *usin;

    int pid = current->pid; 
    syscall_get_arguments(current, regs, args);

    uaddr = (struct sockaddr *)args[1];
    usin = (struct sockaddr_in *)uaddr;

    if (usin && usin->sin_family == AF_INET) {
        //__be32 ip = usin->sin_addr.s_addr;
        __be16 port = usin->sin_port;

        printk(KERN_INFO "TCP PID: %d, TCP connect to %pI4:%d\n",
            pid, &usin->sin_addr, ntohs(port));

        // if (ip == in_aton("8.8.8.8") && ntohs(port) == 80) {
        //     printk(KERN_INFO "EDR: Blocking connection to 8.8.8.8:80 for PID %d\n", pid);
        //     set_syscall_return(regs, -EPERM);
        //     override_function_with_return(regs);
        //     return 1;  
        // }
    }

    return 0;
}


static int __kprobes udp_connect_handler(struct kprobe *p, struct pt_regs *regs)
{
    unsigned long args[6];
    struct sockaddr *uaddr;
    struct sockaddr_in *usin;

    int pid = current->pid;
    syscall_get_arguments(current, regs, args);

    uaddr = (struct sockaddr *)args[1];
    usin = (struct sockaddr_in *)uaddr;

    if (usin && usin->sin_family == AF_INET) {
        //__be32 ip = usin->sin_addr.s_addr;
        __be16 port = usin->sin_port;

        printk(KERN_INFO "UDP: PID %d trying to connect to %pI4:%d\n",
               pid, &usin->sin_addr, ntohs(port));

        // if (ip == in_aton("8.8.8.8") && ntohs(port) == 53) {
        //     printk(KERN_INFO "EDR: Blocking UDP connect to 8.8.8.8:53 for PID %d\n", pid);
        //     set_syscall_return(regs, -EPERM); 
        //     override_function_with_return(regs);
        //     return 1;  
        // }
    }

    return 0;
}

int extract_qname(char *out, const uint8_t *data, int len)
{
    int i = 12;  // DNS header size
    int j = 0;

    if (len <= 12 || !out || !data)
        return -EPERM;

    uint16_t flags = (data[2] << 8) | data[3];
    uint16_t qdcount = (data[4] << 8) | data[5];
    if ((flags & 0x8000) != 0 || qdcount == 0) {
        return -EPERM;
    }

    while (i < len && data[i] != 0) {
        uint8_t label_len = data[i++];
        if (label_len == 0 || i + label_len > len || j + label_len + 1 >= 256)
            return -EPERM;

        memcpy(&out[j], &data[i], label_len);
        i += label_len;
        j += label_len;
        out[j++] = '.';
    }

    if (j > 0)
        out[j - 1] = '\0';  
    else
        out[0] = '\0';

    return 0;  // Success
}


static int __kprobes udp_sendmsg_handler(struct kprobe *p, struct pt_regs *regs)
{
    struct sock *sk;
    struct inet_sock *inetsk;
    struct msghdr *msg;
    unsigned long args[6];
    int length    = 0;
    struct event_node *node;
    //size_t i = 0;

    syscall_get_arguments(current, regs, args);
    sk = (struct sock *)args[0];
    msg = (struct msghdr *)args[1];
    length = (int)args[2];

    if(!msg || !sk){
        printk(KERN_INFO "UDP: Invalid arguments (sk/msg null)\n");
        return 0;
    }

    if(sk->sk_protocol != IPPROTO_UDP) {
        printk(KERN_INFO "UDP: Invalid arguments protocol != IPPROTO_UDP\n");
        return 0;
    }

    inetsk = inet_sk(sk);

    if(!inetsk ) {
        printk(KERN_INFO "UDP: Invalid arguments inetsk\n");
        return 0;
    }

    printk(KERN_INFO "UDP: udp_sendmsg from IP: %pI4, port: %d\n", &inetsk->inet_saddr, ntohs(inetsk->inet_sport));
    printk(KERN_INFO "UDP: udp_sendmsg to IP: %pI4, port: %d\n", &inetsk->inet_daddr, ntohs(inetsk->inet_dport));

    if (msg->msg_iter.iov->iov_base && length > 0) {
        char *buffer = kmalloc(length, GFP_KERNEL);
        node = kmalloc(sizeof(*node), GFP_ATOMIC);

        if (!buffer || !node) {
            printk(KERN_ERR "UDP: kmalloc failed for length = %u\n", length);
            return 0;
        }

        node->evt.pid = current->pid;
        node->evt.timestamp_ns = ktime_get_ns();
        node->evt.len = length;
        
        if (copy_from_user(buffer, msg->msg_iter.iov->iov_base, length)) {
            printk(KERN_ERR "UDP: Failed to copy data from user buffer\n");
            kfree(buffer);
            return 0;
        }

        char qname[128] = {};
        if(ntohs(inetsk->inet_dport) == 53){
            if (extract_qname(qname, buffer, length) == 0) {
                printk(KERN_INFO "DNS: QNAME = %s\n", qname);
            }else {
                printk(KERN_INFO "DNS: Data = %s , length: %d\n", buffer, length);
                // for (i = 0; i < length; i++) {
                //     printk(KERN_INFO " %02x", (unsigned char)buffer[i]);
                // }
            }
        }else {
            printk(KERN_INFO "UDP: Data = %s , length: %d\n", buffer, length);
            // for (i = 0; i < length; i++) {
            //     printk(KERN_INFO " %02x", (unsigned char)buffer[i]);
            // }
        }

        spin_lock(&event_lock);
        list_add_tail(&node->list, &event_list);
        spin_unlock(&event_lock);

        wake_up_interruptible(&event_wait);


        kfree(buffer);
    }

    return 0;
}

static int __kprobes udpv6_sendmsg_handler(struct kprobe *p, struct pt_regs *regs)
{
    struct sock *sk;
    struct msghdr *msg;
    struct sockaddr_in *usin;
    unsigned long args[6];

    syscall_get_arguments(current, regs, args);
    sk = (struct sock *)args[0];
    msg = (struct msghdr *)args[1];

    // if (msg->msg_name) {
    //     usin = (struct sockaddr_in *)msg->msg_name;
    //     if(usin) {
    //         __be32 daddr = usin->sin_addr.s_addr;
    //         __be16 dport = usin->sin_port;
    //         printk(KERN_INFO "UDPV6: udpv6_sendmsg to IP: %pI6, port: %d\n", &daddr, ntohs(dport));
    //     } else {
    //         printk(KERN_INFO "UDPV6: convert (usin) failed\n");
    //     }
    // }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = edr_ioctl,
    .read = edr_read
};

static struct kprobe file_open_kp = {
    .symbol_name = "do_sys_open",
    .pre_handler = file_open_handler,
};

static struct kprobe file_write_kp = {
    .symbol_name = "vfs_write",
    .pre_handler = file_write_handler,
};

static struct kprobe file_read_kp = {
    .symbol_name = "vfs_read",
    .pre_handler = file_read_handler
};

static struct kprobe file_rename_kp = {
    .symbol_name = "vfs_rename",
    .pre_handler = file_rename_handler
};

static struct kprobe file_unlink_kp = {
    .symbol_name = "vfs_unlink",
    .pre_handler = file_unlink_handler
};

static struct kprobe file_execve_kp = {
    .symbol_name = "bprm_execve",
    .pre_handler = file_execve_handler
};

static struct kprobe kill_kp = {
    .symbol_name = "kill_pid_info",
    .pre_handler = kill_handler
};

static struct kprobe module_delete_kp = {
    .symbol_name = "try_release_module_ref",
    .pre_handler = module_delete_handler
};

// static struct kprobe module_delete_kp = {
//     .symbol_name = "free_module",
//     .pre_handler = module_delete_handler
// };

static struct kprobe module_find_kp = {
    .symbol_name = "find_module_all",
    .pre_handler = module_find_handler
};

static struct kprobe module_init_kp = {
    .symbol_name = "do_init_module",
    .pre_handler = module_init_handler
};

static struct kprobe tcp_kp = {
    .symbol_name = "tcp_v4_connect",
    .pre_handler = tcp_connect_handler
};


static struct kprobe udp_kp = {
    .symbol_name = "ip4_datagram_connect",
    .pre_handler = udp_connect_handler
};

static struct kprobe udp_sendmsg_kp = {
    .symbol_name = "udp_sendmsg",
    .pre_handler = udp_sendmsg_handler
};

static struct kprobe udpv6_sendmsg_kp = {
    .symbol_name = "udpv6_sendmsg",
    .pre_handler = udpv6_sendmsg_handler
};


static int __init kprobe_init(void)
{
    int ret;

    ret = register_kprobe(&module_delete_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&module_init_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&module_find_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&file_open_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&file_write_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&file_read_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&file_rename_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&file_unlink_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&file_execve_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&kill_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&tcp_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }
    
    ret = register_kprobe(&udp_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&udp_sendmsg_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }

    ret = register_kprobe(&udpv6_sendmsg_kp);
    if (ret < 0) {
        printk(KERN_ERR "EDR: Failed to register kprobe: %d\n", ret);
        return ret;
    }


    /*register class*/
    alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    edr_class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(edr_class, NULL, dev_number, NULL, DEVICE_NAME);

    /*register device*/
    cdev_init(&edr_cdev, &fops);
    cdev_add(&edr_cdev, dev_number, 1);

    printk(KERN_INFO "EDR: Kprobe registered for device file edr\n");
    return 0;
}

static void __exit kprobe_exit(void)
{
    struct event_node *node, *tmp;

    /*close device file and class*/
    device_destroy(edr_class, dev_number);
    class_destroy(edr_class);
    cdev_del(&edr_cdev);
    unregister_chrdev_region(dev_number, 1);

    /*destroy kprobe*/
    unregister_kprobe(&file_open_kp);
    unregister_kprobe(&file_write_kp);
    unregister_kprobe(&file_read_kp);
    unregister_kprobe(&file_rename_kp);
    unregister_kprobe(&file_unlink_kp);
    unregister_kprobe(&file_execve_kp);
    unregister_kprobe(&kill_kp);
    unregister_kprobe(&module_delete_kp);
    unregister_kprobe(&module_find_kp);
    unregister_kprobe(&module_init_kp);
    unregister_kprobe(&tcp_kp);
    unregister_kprobe(&udp_kp);
    unregister_kprobe(&udp_sendmsg_kp);
    unregister_kprobe(&udpv6_sendmsg_kp);

    spin_lock(&event_lock);
    list_for_each_entry_safe(node, tmp, &event_list, list) {
        list_del(&node->list);
        kfree(node);
    }
    spin_unlock(&event_lock);

    printk(KERN_INFO "EDR: Kprobe unregistered\n");
}

module_init(kprobe_init);
module_exit(kprobe_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("buihien29112002@gmail.com");
MODULE_DESCRIPTION("EDR Kprobe Module for Blocking Events");