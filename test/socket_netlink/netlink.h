#ifndef _NET_LINK_H
#define _NET_LINK_H

#include <linux/types.h> 

#define NAME_MAX 255
#define PATH_MAX 4096
#define TASK_COMM_LEN 16

#define NETLINK_EDR 29

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

struct edr_netlink_cmd {
    char cmd[TASK_COMM_LEN]; 
    int pid;
};


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



#endif