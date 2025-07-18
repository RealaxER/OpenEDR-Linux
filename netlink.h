#ifndef _NET_LINK_H
#define _NET_LINK_H

#include <linux/types.h> 

#define NAME_MAX 255
#define PATH_MAX 4096
#define TASK_COMM_LEN 16

#define NETLINK_EDR 29

#define HOOK_OPEN_STR     "open"
#define HOOK_READ_STR     "read"
#define HOOK_WRITE_STR    "write"
#define HOOK_UNLINK_STR   "unlink"

#define HOOK_OPEN_INDEX 0
#define HOOK_READ_INDEX 1
#define HOOK_WRITE_INDEX 2
#define HOOK_UNLINK_INDEX  3

#define HOOK_OPEN_BIT     (1ULL << HOOK_OPEN_INDEX)
#define HOOK_READ_BIT     (1ULL << HOOK_READ_INDEX)
#define HOOK_WRITE_BIT    (1ULL << HOOK_WRITE_INDEX)
#define HOOK_UNLINK_BIT   (1ULL << HOOK_UNLINK_INDEX)

struct edr_event {
    char event[TASK_COMM_LEN];          
    pid_t pid;                          
    pid_t ppid;                         
    pid_t tgid;                         
    uid_t uid;                          
    char comm[TASK_COMM_LEN];     

    char *fname;               
    char *path;                

    int flags;                          
    long result;                        
    long timestamp_ns;       
}__attribute__((packed));


#define setup_event(event, name) do { \
    (event).pid = current->pid; \
    (event).tgid = current->tgid; \
    (event).uid = from_kuid(&init_user_ns, current_uid()); \
    (event).ppid = task_ppid_nr(current); \
    strncpy(event.comm, current->comm, TASK_COMM_LEN);\
    strncpy(event.event, name, TASK_COMM_LEN);\
    (event).timestamp_ns = ktime_get_ns();\
    (event).fname = NULL;\
    (event).path = NULL;\
    (event).timestamp_ns = ktime_get_ns();\
} while (0)

struct edr_event_hdr {
    char event[TASK_COMM_LEN];          
    pid_t pid;                          
    pid_t ppid;                         
    pid_t tgid;                         
    uid_t uid;                          
    char comm[TASK_COMM_LEN];           

    __u16 fname_offset;              
    __u16 path_offset;                  

    int flags;                          
    long result;                        
    long timestamp_ns;                  

    __u16 total_size;                   
} __attribute__((packed));


#define COMMAND_NONE -1
#define COMMAND_AND   0
#define COMMAND_OR    1

#define OPERATOR_EQUALS 0
#define OPERATOR_IN     1

#define FLAG_NONE   0
#define FLAG_RULE   (1ULL << 0)
#define FLAG_MACRO  (1ULL << 1)
#define FLAG_LIST  (1ULL << 2)


#define MAX_FIELD_SIZE  64
#define MAX_VALUE_SIZE  64
#define MAX_COMMAND_RULE 10

#define MAX_RULES 100
#define MAX_MACRO 100
#define MAX_LIST 100

#define MAX_HOOKS 10
#define MAX_TAGS MAX_HOOKS
#define MAX_ITEMS MAX_HOOKS

#define MAX_STRING_LEN 256

#define EDR_EVENT_SET (1ULL << 0)
#define EDR_EVENT_CLEAR (1ULL << 1)
#define EDR_EVENT_CHECK_PATH (1ULL << 2)

enum edr_action {
    EDR_ACTION_MONITOR = 0,
    EDR_ACTION_BLOCK   = 1
};

struct command {
    int flag;                 // AND / OR / NONE
    int operator;             // == / in
    char field[MAX_FIELD_SIZE]; 
    char value[MAX_VALUE_SIZE]; 
};

struct edr_event_cmd {
    __u8 flags;
    char id[TASK_COMM_LEN];  
    __u16 fname_offset;              
    __u16 path_offset;         
    struct command command[MAX_COMMAND_RULE];    
    char hooked [MAX_COMMAND_RULE][TASK_COMM_LEN];
    __u8 command_count;
    __u8 hooked_count;
    __u8 action; 
    __u16 total_size;                   
} __attribute__((packed));

#endif