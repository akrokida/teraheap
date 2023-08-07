#ifndef __SIMPLE_URING_H__
#define __SIMPLE_URING_H__

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <liburing.h>

#define QD 4

struct io_data {
        off_t first_offset, offset;
        size_t first_len;
        struct iovec iov;
};


// inits the queue
int setup_context(unsigned queue_depth);

// basic io_uring write and returns the number of pending writes
int queue_write(int fd, struct io_data *data);

// implements multiple writes, takes the data (char *s), number of writes (int writes) and the wanted offset
int m_write(int writes, int fd, char *src, int len, int offset);

int setup_after(void);


#endif


