
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt


#include "netlink.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

#define NETLINK_EDR 31

static struct sock *nl_sk = NULL;
static int user_pid = 0;

static void send_edr_event(void)
{
    const char *fname = "/etc/passwd";
    const char *path = "/etc";
    size_t fname_len = strlen(fname) + 1;
    size_t path_len = strlen(path) + 1;

    size_t total_size = sizeof(struct edr_event_hdr) + fname_len + path_len;

    char *buffer = kmalloc(total_size, GFP_KERNEL);
    if (!buffer) {
        pr_warn("EDR: kmalloc failed\n");
        return;
    }

    struct edr_event_hdr *hdr = (struct edr_event_hdr *)buffer;

    strncpy(hdr->event, "open", TASK_COMM_LEN);
    hdr->pid = current->pid;
    hdr->ppid = current->real_parent->pid;
    hdr->tgid = current->tgid;
    hdr->uid = current_uid().val;
    strncpy(hdr->comm, current->comm, TASK_COMM_LEN);

    hdr->fname_offset = sizeof(*hdr);
    strcpy(buffer + hdr->fname_offset, fname);

    hdr->path_offset = hdr->fname_offset + fname_len;
    strcpy(buffer + hdr->path_offset, path);

    hdr->flags = 0;
    hdr->result = 0;
    hdr->timestamp_ns = ktime_get_ns();
    hdr->total_size = total_size;

    struct sk_buff *skb = nlmsg_new(total_size, GFP_KERNEL);
    if (!skb) {
        pr_warn("EDR: nlmsg_new failed\n");
        kfree(buffer);
        return;
    }

    struct nlmsghdr *nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, total_size, 0);
    memcpy(nlmsg_data(nlh), buffer, total_size);
    kfree(buffer);

    netlink_unicast(nl_sk, skb, user_pid, 0);
}

static void netlink_recv(struct sk_buff *skb)
{
    struct nlmsghdr *nlh = nlmsg_hdr(skb);
    user_pid = nlh->nlmsg_pid;
    pr_info("EDR: User PID %d registered.\n", user_pid);
    send_edr_event();
}

static int __init edr_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = netlink_recv,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_EDR, &cfg);
    if (!nl_sk) {
        pr_err("EDR: Cannot create netlink socket\n");
        return -ENOMEM;
    }

    pr_info("EDR: Netlink module loaded\n");
    return 0;
}

static void __exit edr_exit(void)
{
    netlink_kernel_release(nl_sk);
    pr_info("EDR: Netlink module unloaded\n");
}

module_init(edr_init);
module_exit(edr_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hiển Bùi");
MODULE_DESCRIPTION("EDR Netlink Example with Offset-based Struct");
