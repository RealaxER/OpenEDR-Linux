#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdint.h>
#include "netlink.h"

#define NETLINK_EDR 29
#define MAX_PAYLOAD 1024

int sock_fd;
struct sockaddr_nl dest_addr;

void send_cmd_to_kernel(const char *cmd) {
    struct nlmsghdr *nlh;
    struct edr_netlink_cmd payload;

    struct iovec iov;
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    memset(&payload, 0, sizeof(payload));

    strncpy(payload.cmd, cmd, sizeof(payload.cmd) - 1);
    payload.pid = getpid();

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(payload)));
    memset(nlh, 0, NLMSG_SPACE(sizeof(payload)));
    nlh->nlmsg_len = NLMSG_SPACE(sizeof(payload));
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    memcpy(NLMSG_DATA(nlh), &payload, sizeof(payload));

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    sendmsg(sock_fd, &msg, 0);
    free(nlh);
}

void handle_sigint(int sig) {
    printf("\nCaught SIGINT. Sending stop to kernel...\n");
    send_cmd_to_kernel("stop");
    close(sock_fd);
    exit(0);
}

int main() {
    struct sockaddr_nl src_addr = {0};
    char buffer[MAX_PAYLOAD + NLMSG_HDRLEN];

    signal(SIGINT, handle_sigint);

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_EDR);
    if (sock_fd < 0) {
        perror("socket");
        return -1;
    }

    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();  // This process PID
    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // kernel
    dest_addr.nl_groups = 0;

    printf("Sending start to kernel...\n");
    send_cmd_to_kernel("start");

    printf("Waiting for EDR events...\n");

    while (1) {
        ssize_t len = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (len <= 0) continue;

        struct nlmsghdr *nlmsg = (struct nlmsghdr *)buffer;
        struct edr_event_hdr *hdr = (struct edr_event_hdr *)NLMSG_DATA(nlmsg);

        char *filename = ((char *)hdr) + hdr->fname_offset;
        char *path = ((char *)hdr) + hdr->path_offset;

        printf("\n[EDR EVENT]\n");
        printf("  %-10s: %s\n", "Event", hdr->event);
        printf("  %-10s: %d\n", "PID", hdr->pid);
        printf("  %-10s: %d\n", "PPID", hdr->ppid);
        printf("  %-10s: %d\n", "TGID", hdr->tgid);
        printf("  %-10s: %d\n", "UID", hdr->uid);
        printf("  %-10s: %s\n", "Comm", hdr->comm);
        printf("  %-10s: 0x%x\n", "Flags", hdr->flags);
        printf("  %-10s: %ld\n", "Result", hdr->result);
        printf("  %-10s: %ld\n", "Time(ns)", hdr->timestamp_ns);
        printf("  %-10s: %s\n", "Filename", filename);
        printf("  %-10s: %s\n", "Path", path);
    }

    return 0;
}
