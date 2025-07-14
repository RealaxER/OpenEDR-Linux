#ifndef __EDR_IOCTL_H__
#define __EDR_IOCTL_H__

#define EDR_IOCTL_MAGIC 'E'

struct edr_data {
    int id;
    char name[32];
};

#define EDR_IOCTL_SEND_DATA _IOW(EDR_IOCTL_MAGIC, 1, struct edr_data)
#define EDR_MAX_ARRAY 10
#define EDR_IOCTL_SEND_ARRAY _IOW(EDR_IOCTL_MAGIC, 2, struct edr_data *)
#endif
