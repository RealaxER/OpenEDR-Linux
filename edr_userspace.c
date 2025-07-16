#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <time.h>

#include "netlink.h"

#define NETLINK_EDR 29
#define MAX_PAYLOAD 1024
#define LOG_FILE_PATH "edr_log.txt"
#define MAX_COMM_TRACKED 1024

int sock_fd;
struct sockaddr_nl dest_addr;
FILE *log_file = NULL;

// Track list of seen comms
char *seen_comms[MAX_COMM_TRACKED];
int seen_comm_count = 0;

int already_logged_comm(const char *comm) {
    for (int i = 0; i < seen_comm_count; i++) {
        if (strcmp(seen_comms[i], comm) == 0) {
            return 1; // already exists
        }
    }
    return 0;
}

void add_logged_comm(const char *comm) {
    if (seen_comm_count >= MAX_COMM_TRACKED) return;

    seen_comms[seen_comm_count++] = strdup(comm);  // Remember to free if needed
}

const char *common_comms[] = {
    "bash", "node", "sh", "which", "ps", "git", "sed", "cat", "sleep",
    "systemd-journal", "auditd", "sshd", "in:imklog", "rs:main Q:Reg",
    "gmain", "tokio-runtime-w", "dmesg", "irqbalance", "rtkit-daemon"
};

int is_common_comm(const char *comm) {
    int n = sizeof(common_comms) / sizeof(common_comms[0]);
    for (int i = 0; i < n; i++) {
        if (strcmp(comm, common_comms[i]) == 0)
            return 1;
    }
    return 0;
}

void send_edr_event_cmd_to_kernel(struct edr_event_cmd *cmd, const char *fname, const char *path) {
    struct nlmsghdr *nlh;
    struct iovec iov;
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));

    size_t fname_len = strlen(fname) + 1;
    size_t path_len  = strlen(path) + 1;

    cmd->fname_offset = sizeof(struct edr_event_cmd);
    cmd->path_offset  = cmd->fname_offset + fname_len;
    cmd->total_size   = cmd->path_offset + path_len;

    nlh = malloc(NLMSG_SPACE(cmd->total_size));
    memset(nlh, 0, NLMSG_SPACE(cmd->total_size));
    nlh->nlmsg_len = NLMSG_SPACE(cmd->total_size);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    void *payload = NLMSG_DATA(nlh);
    memcpy(payload, cmd, sizeof(struct edr_event_cmd));
    memcpy((char *)payload + cmd->fname_offset, fname, fname_len);
    memcpy((char *)payload + cmd->path_offset, path, path_len);

    iov.iov_base = nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    sendmsg(sock_fd, &msg, 0);
    free(nlh);
}

void send_cmd_to_kernel_example(void){
    struct edr_event_cmd cmd;
    memset(&cmd, 0, sizeof(cmd));

    // ID dùng để track hoặc cancel rule
    strncpy(cmd.id, "rule_01", TASK_COMM_LEN);

    // 1 rule: uid == 0
    cmd.command[0].flag = COMMAND_NONE;
    cmd.command[0].operator = OPERATOR_EQUALS;
    strncpy(cmd.command[0].field, "uid", MAX_FIELD_SIZE);
    strncpy(cmd.command[0].value, "0", MAX_VALUE_SIZE);

    // 2 rule: comm in bash,zsh
    cmd.command[1].flag = COMMAND_AND;
    cmd.command[1].operator = OPERATOR_IN;
    strncpy(cmd.command[1].field, "comm", MAX_FIELD_SIZE);
    strncpy(cmd.command[1].value, "bash", MAX_VALUE_SIZE);

    // Hooked hành vi
    strncpy(cmd.hooked[0], "open", TASK_COMM_LEN);
    strncpy(cmd.hooked[1], "read", TASK_COMM_LEN);
    strncpy(cmd.hooked[2], "write", TASK_COMM_LEN);
    strncpy(cmd.hooked[3], "unlink", TASK_COMM_LEN);

    // Gửi xuống kernel
    send_edr_event_cmd_to_kernel(&cmd, "shadow", "/etc/shadow");
}

void handle_sigint(int sig) {
    printf("\nCaught SIGINT. Sending stop to kernel...\n");

    // if (log_file) {
    //     fclose(log_file);
    // }

    for (int i = 0; i < seen_comm_count; i++) {
        free(seen_comms[i]);
    }

    close(sock_fd);
    exit(0);
}

void log_event_to_file(struct edr_event_hdr *hdr, const char *filename, const char *path) {
    if (!log_file) return;

    // if (already_logged_comm(hdr->comm)) {
    //     return;
    // }

    // // Save this comm to prevent re-logging
    // add_logged_comm(hdr->comm);

    // if (is_common_comm(hdr->comm)) return; // BỎ QUA tiến trình phổ biến


    // Get current time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestr[64];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log_file,
        "[%s] [%s] pid=%d, tgid=%d, ppid=%d, uid=%d, comm=%s\n"
        "         fname=%s, path=%s, flags=0x%x, result=%ld, timestamp=%ld\n\n",
        timestr,
        hdr->event, hdr->pid, hdr->tgid, hdr->ppid, hdr->uid, hdr->comm,
        filename, path, hdr->flags, hdr->result, hdr->timestamp_ns);

    fflush(log_file);  // Ghi ngay lập tức
}

int main() {
    struct sockaddr_nl src_addr = {0};
    char buffer[MAX_PAYLOAD + NLMSG_HDRLEN];

    signal(SIGINT, handle_sigint);

    log_file = fopen(LOG_FILE_PATH, "a");
    if (!log_file) {
        perror("Failed to open log file");
        return 1;
    }

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

    send_cmd_to_kernel_example();

    printf("Waiting for EDR events...\n");

    while (1) {
        ssize_t len = recv(sock_fd, buffer, sizeof(buffer), 0);
        if (len <= 0) continue;

        struct nlmsghdr *nlmsg = (struct nlmsghdr *)buffer;
        struct edr_event_hdr *hdr = (struct edr_event_hdr *)NLMSG_DATA(nlmsg);

        char *filename = ((char *)hdr) + hdr->fname_offset;
        char *path = ((char *)hdr) + hdr->path_offset;

        // log_event_to_file(hdr, filename, path);
    }

    return 0;
}
