#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <time.h>
#include <yaml.h>
#include <stdbool.h>
#include "../../netlink.h"

#define NETLINK_EDR 29
#define MAX_PAYLOAD 1024
#define LOG_FILE_PATH "edr_log.txt"
#define MAX_COMM_TRACKED 1024

int sock_fd;
struct sockaddr_nl dest_addr;
FILE *log_file = NULL;

typedef struct {
    char name[MAX_STRING_LEN];
    char id[TASK_COMM_LEN];
    
    char description[1024];
    char path[1024];
    char fname[MAX_STRING_LEN];
    char action[TASK_COMM_LEN];
    char priority[TASK_COMM_LEN];
    char output[MAX_STRING_LEN];
    char condition[MAX_STRING_LEN];
    
    char hooked[MAX_HOOKS][TASK_COMM_LEN];
    int hooked_count;
    
    char tags[MAX_TAGS][TASK_COMM_LEN];
    int tags_count;
} edr_rule_t;

typedef struct {
    char condition[MAX_STRING_LEN];
    char name[TASK_COMM_LEN];
} edr_macro_t;

typedef struct {
    char comm[TASK_COMM_LEN][TASK_COMM_LEN];
    char name[TASK_COMM_LEN];
    int item_count;
} edr_list_t;

edr_rule_t rules[MAX_RULES];
edr_macro_t macros[MAX_MACRO];
edr_list_t lists[MAX_LIST];

int rule_count = 0;
int macro_count = 0;
int list_count = 0;
uint8_t parse_flag = 0;

char current_key[128] = {0};
int expect_value = 0;
int parsing_hooked_array = 0;
int parsing_tags_array = 0;
int parsing_items_array = 0;

unsigned long long flag = 0;

void process_scalar(yaml_token_t token) {
    edr_rule_t *current_rule = &rules[rule_count];
    edr_macro_t *current_macro = &macros[macro_count];
    edr_list_t *current_list = &lists[list_count];

    if (parsing_hooked_array) {
        if (current_rule->hooked_count < MAX_HOOKS) {
            strncpy(current_rule->hooked[current_rule->hooked_count++], (char*)token.data.scalar.value, 64 - 1);
        }
        return;
    }
    if (parsing_tags_array) {
        if (current_rule->tags_count < MAX_TAGS) {
            strncpy(current_rule->tags[current_rule->tags_count++], (char*)token.data.scalar.value, 64 - 1);
        }
        return;
    }

    if(parsing_items_array) {
        if (current_list->item_count < MAX_ITEMS) {
            strncpy(current_list->comm[current_list->item_count++], (char*)token.data.scalar.value, TASK_COMM_LEN - 1);
        }
        return;
    }

    if (!expect_value) { 
        strncpy(current_key, (char *)token.data.scalar.value, sizeof(current_key) - 1);
    } else {
        if (strcmp(current_key, "rule") == 0) {
            if (rules[rule_count].name[0] != '\0') {
                if (rule_count < MAX_RULES - 1) {
                    rule_count++;
                }
            }
            memset(&rules[rule_count], 0, sizeof(edr_rule_t));
            rules[rule_count].hooked_count = 0;
            rules[rule_count].tags_count = 0;
            parse_flag = FLAG_RULE;

            strncpy(current_rule->name, (char*)token.data.scalar.value, sizeof(current_rule->name) - 1);
        } else if (strcmp(current_key, "id") == 0) {
            strncpy(current_rule->id, (char*)token.data.scalar.value, sizeof(current_rule->id) - 1);
        } else if (strcmp(current_key, "des") == 0) {
            strncpy(current_rule->description, (char*)token.data.scalar.value, sizeof(current_rule->description) - 1);
        } else if (strcmp(current_key, "path") == 0) {
            strncpy(current_rule->path, (char*)token.data.scalar.value, sizeof(current_rule->path) - 1);
        } else if (strcmp(current_key, "fname") == 0) {
            strncpy(current_rule->fname, (char*)token.data.scalar.value, sizeof(current_rule->fname) - 1);
        } else if (strcmp(current_key, "action") == 0) {
            strncpy(current_rule->action, (char*)token.data.scalar.value, sizeof(current_rule->action) - 1);
        } else if (strcmp(current_key, "priority") == 0) {
            strncpy(current_rule->priority, (char*)token.data.scalar.value, sizeof(current_rule->priority) - 1);
        } else if (strcmp(current_key, "output") == 0) {
            strncpy(current_rule->output, (char*)token.data.scalar.value, sizeof(current_rule->output) - 1);
        } else if (strcmp(current_key, "condition") == 0) {
            if (parse_flag & FLAG_MACRO) {
                strncpy(current_macro->condition, (char*)token.data.scalar.value, sizeof(current_macro->condition) - 1);
            } else if (parse_flag & FLAG_RULE) {
                strncpy(current_rule->condition, (char*)token.data.scalar.value, sizeof(current_rule->condition) - 1);
            }
        } else if (strcmp(current_key, "macro") == 0) {
            if (macros[macro_count].name[0] != '\0') {
                if (macro_count < MAX_MACRO - 1) {
                    macro_count++;
                }
            }
            parse_flag = FLAG_MACRO;
            memset(&macros[macro_count], 0, sizeof(edr_macro_t));
            strncpy(current_macro->name, (char*)token.data.scalar.value, sizeof(current_macro->name) - 1);
        }

        else if (strcmp(current_key, "list") == 0) {
            if (lists[list_count].name[0] != '\0') {
                if (list_count < MAX_LIST - 1) {
                    list_count++;
                }
            }
            parse_flag = FLAG_LIST;
            memset(&lists[list_count], 0, sizeof(edr_list_t));
            strncpy(current_list->name, (char*)token.data.scalar.value, sizeof(current_list->name) - 1);
        }
    }
}

int parse_conditions(const char *input_str, struct command *cmds, int max_cmds) {
    char buffer[MAX_STRING_LEN];
    strncpy(buffer, input_str, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *tokens[MAX_STRING_LEN];
    int token_count = 0;

    char *token = strtok(buffer, " ");
    while (token && token_count < MAX_STRING_LEN) {
        tokens[token_count++] = token;
        token = strtok(NULL, " ");
    }

    int i = 0, cmd_count = 0;
    int current_flag = COMMAND_NONE;

    while (i < token_count - 2 && cmd_count < max_cmds) {
        char *field = tokens[i];
        char *op = tokens[i + 1];
        char *val = tokens[i + 2];

        struct command *c = &cmds[cmd_count];
        c->flag = current_flag;

        strncpy(c->field, field, MAX_FIELD_SIZE - 1);
        c->field[MAX_FIELD_SIZE - 1] = '\0';

        strncpy(c->value, val, MAX_VALUE_SIZE - 1);
        c->value[MAX_VALUE_SIZE - 1] = '\0';

        if (strcmp(op, "==") == 0) {
            c->operator = OPERATOR_EQUALS;
        } else if (strcmp(op, "in") == 0) {
            c->operator = OPERATOR_IN;
        } else {
            fprintf(stderr, "Unknown operator: %s\n", op);
            return -1;
        }

        cmd_count++;
        i += 3;

        if (i < token_count) {
            if (strcmp(tokens[i], "and") == 0) {
                current_flag = COMMAND_AND;
            } else if (strcmp(tokens[i], "or") == 0) {
                current_flag = COMMAND_OR;
            } else {
                current_flag = COMMAND_NONE;
            }

            if(c->flag == COMMAND_NONE) {
                c->flag = current_flag;
            }
            i++;
        }
    }

    return cmd_count;
}


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

bool path_has_slash_star(const char *path) {
    size_t len = strlen(path); 

    if (len >= 2 && path[len - 2] == '/' && path[len - 1] == '*') {
        return true;
    }
    return false;
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

void remove_asterisk(char *path) {
    char *slash_asterisk_pos = strstr(path, "/*"); 
    if (slash_asterisk_pos != NULL) {
        *slash_asterisk_pos = '\0'; 
    }
}

bool contains_macro(const char *condition, edr_macro_t macros, char *found_macro) {
    char pattern[TASK_COMM_LEN + 3]; 
    snprintf(pattern, sizeof(pattern), "(%s)", macros.name);
    if (strstr(condition, pattern)) {
        if (found_macro) strncpy(found_macro, macros.name, TASK_COMM_LEN);
        return true;
    }
    return false;
}

bool expand_macro(char *condition, edr_macro_t macros) {
    char expanded[MAX_STRING_LEN * 2] = {0};
    char *macro_start, *macro_end;
    char pattern[TASK_COMM_LEN + 3];
    snprintf(pattern, sizeof(pattern), "(%s)", macros.name);

    macro_start = strstr(condition, pattern);
    if (macro_start) {
        int prefix_len = macro_start - condition;
        strncpy(expanded, condition, prefix_len);
        expanded[prefix_len] = '\0';

        strncat(expanded, macros.condition, MAX_STRING_LEN);

        macro_end = macro_start + strlen(pattern);
        strncat(expanded, macro_end, MAX_STRING_LEN);

        strncpy(condition, expanded, MAX_STRING_LEN);
        return true;
    }

    return false;
}


// void expand_list_names(char *condition, size_t buf_len, struct list *lists, int total_list)
// {
//     std::string cond_str = condition;
//     size_t pos = 0;

//     while ((pos = cond_str.find("{", pos)) != std::string::npos) {
//         size_t end = cond_str.find("}", pos);
//         if (end == std::string::npos) break;

//         std::string list_name = cond_str.substr(pos + 1, end - pos - 1);
//         std::string key_expr = cond_str.substr(0, pos);
//         std::string key;

//         // Tìm phần tử list theo tên
//         struct list *matched_list = NULL;
//         for (int i = 0; i < total_list; ++i) {
//             if (strcmp(lists[i].name, list_name.c_str()) == 0) {
//                 matched_list = &lists[i];
//                 break;
//             }
//         }

//         if (!matched_list) {
//             pos = end + 1;
//             continue;
//         }

//         // Lấy key như "comm in" hoặc "name in"
//         size_t key_start = key_expr.rfind("name");
//         if (key_start == std::string::npos)
//             key_start = key_expr.rfind("comm");
//         if (key_start == std::string::npos)
//             break;

//         key = key_expr.substr(key_start);
//         key.erase(key.find_last_not_of(" \t") + 1); // Trim trailing spaces

//         std::string expanded;

//         if (key.find("==") != std::string::npos) {
//             // comm == {shells} -> comm == sh (chỉ lấy phần tử đầu)
//             expanded = key.substr(0, key.find("==") + 2) + " " + matched_list->comm[0];
//         } else if (key.find("in") != std::string::npos) {
//             // comm in {shells} -> comm in sh or comm in bash ...
//             for (int i = 0; i < matched_list->item_count; ++i) {
//                 if (i != 0)
//                     expanded += " or ";
//                 expanded += key.substr(0, key.find("in") + 2) + " " + matched_list->comm[i];
//             }
//         }

//         // Thay thế đoạn "{shells}" trong chuỗi gốc
//         cond_str.replace(key_start, end - key_start + 1, expanded);
//         pos = key_start + expanded.length();
//     }

//     // Copy kết quả cuối cùng về lại condition
//     strncpy(condition, cond_str.c_str(), buf_len - 1);
//     condition[buf_len - 1] = '\0';
// }


void expand_all_macros(char *condition, edr_macro_t *macros, size_t macros_size) {
    char found_macro[TASK_COMM_LEN];


    for(int i= 0; i < macros_size; i++ ){
        if (contains_macro(condition, macros[i], found_macro)) {
            expand_macro(condition, macros[i]);
        }
    }
}

int main() {
    FILE *fh = fopen("config.yaml", "r");
    if (!fh) {
        perror("fopen: config.yaml");
        return 1;
    }
    yaml_parser_t parser;
    yaml_token_t token;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, fh);

    int done = 0;
    while (!done) {
        if (!yaml_parser_scan(&parser, &token)) {
            fprintf(stderr, "Error parsing YAML\n");
            break;
        }

        switch (token.type) {
        case YAML_STREAM_END_TOKEN:
            done = 1;
            break;

        case YAML_BLOCK_ENTRY_TOKEN: 
            break;

        case YAML_KEY_TOKEN:
            expect_value = 0;
            break;
        case YAML_VALUE_TOKEN:
            expect_value = 1;
            break;

        case YAML_SCALAR_TOKEN:
            process_scalar(token);
            break;

        case YAML_FLOW_SEQUENCE_START_TOKEN: 
            if (strcmp(current_key, "hooked") == 0) parsing_hooked_array = 1;
            else if (strcmp(current_key, "tags") == 0) parsing_tags_array = 1;
            else if (strcmp(current_key, "items") == 0) parsing_items_array = 1;
            break;
        
        case YAML_FLOW_SEQUENCE_END_TOKEN:
            parsing_hooked_array = 0;
            parsing_tags_array = 0;
            parsing_items_array = 0;
            break;

        default:
            break;
        }

        yaml_token_delete(&token);
    }

    yaml_parser_delete(&parser);
    fclose(fh);

    int total_rules = rule_count + 1;
    if (rules[0].name[0] == '\0') total_rules = 0; 

    int total_macro = macro_count +1;
    if (macros[0].name[0] == '\0') total_macro = 0; 

    int total_list = list_count +1;
    if (lists[0].name[0] == '\0') total_list = 0; 

    if(total_list > 0){
        printf("total_list: %d\n", total_list);
        printf("name: %s\n", lists[0].name);
        printf("com: %s\n", lists[0].comm[0]);
        printf("com: %s\n", lists[0].comm[1]);
    }

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

    struct edr_event_cmd event;
    memset(&event, 0, sizeof(event));

    event.flags = EDR_EVENT_CLEAR;
    send_edr_event_cmd_to_kernel(&event, "", "");

    event.flags = EDR_EVENT_SET;

    printf("--- PARSED %d RULES ---\n\n", total_rules);
    for (int i = 0; i < total_rules; i++) {
        struct command cmds[MAX_COMMAND_RULE];

        if(strstr(rules[i].action, "block")) {
            event.action = EDR_ACTION_BLOCK;
        }else {
            event.action = EDR_ACTION_MONITOR;
        }

        printf("before rules[i].condition: %s\n", rules[i].condition);

        //expand_list_names(rules[i].condition, strlen(rules[i].condition), lists, total_list);        
        expand_all_macros(rules[i].condition, macros, total_macro);

        printf("after rules[i].condition: %s\n", rules[i].condition);

        int command_count = parse_conditions(rules[i].condition, cmds, MAX_COMMAND_RULE);

        strncpy(event.id, rules[i].id, TASK_COMM_LEN);
        event.id[TASK_COMM_LEN - 1] = '\0';
        memcpy(event.command, cmds, sizeof(struct command) * command_count);

        memcpy(event.hooked, rules[i].hooked, sizeof(char) * TASK_COMM_LEN * rules[i].hooked_count);
        event.hooked_count = rules[i].hooked_count;
        event.command_count = command_count;

        printf("path before: %s\n", rules[i].path);

        if(path_has_slash_star(rules[i].path)) {
            event.flags |= EDR_EVENT_CHECK_PATH;
            remove_asterisk(rules[i].path);
            printf("path after: %s\n", rules[i].path);
        }else {
            event.flags = EDR_EVENT_SET;
        }

        send_edr_event_cmd_to_kernel(&event, rules[i].fname, rules[i].path);
    }

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
