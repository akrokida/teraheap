#include "aaa.h"

struct io_uring ring;
int pending;
struct io_data *data[10];

int setup_context(unsigned queue_depth){
        int ret = io_uring_queue_init(queue_depth, &ring, 0);
        if (ret < 0) {
                fprintf(stderr, "queue_init: %s\n", strerror(-ret));
                return -1;
        }

        return 0;
}

int queue_write(int fd, struct io_data *data){
        struct io_uring_sqe *sqe;
        sqe = io_uring_get_sqe(&ring);
        assert(sqe);
	
	io_uring_prep_writev(sqe, fd, &data->iov, 1, data->offset);
        io_uring_sqe_set_data(sqe, data);
	
	pending = io_uring_submit(&ring);
}

int m_write(int writes, int fd, char *src, int len, int offset){
	static int x = 0;
	if(x < 10){
		data[offset % 10] = malloc(sizeof(struct io_data));
		x++;
	}

	data[offset % 10]->first_offset = 0;
	data[offset % 10]->iov.iov_base = malloc(sizeof(struct iovec));
	data[offset % 10]->iov.iov_len = len;
	data[offset % 10]->offset = offset;
	
	static int count = 0;

	memcpy(data[offset % 10]->iov.iov_base, src, 10);
	pending = queue_write(fd, data[offset % 10]);

	count++;
	if(count == writes) setup_after();
}

int setup_after(void){
	struct io_uring_cqe *cqe;
	int wait_ret, i;

	if(pending < 0){
		fprintf(stderr, "io_uring_submit: %s\n", strerror(-pending));
		return 1;
	}

	for(i = 0; i < 0; i++){
		wait_ret = io_uring_wait_cqe(&ring, &cqe);
		if(wait_ret < 0){
			fprintf(stderr, "io_uring_wait_cqe: %s\n", strerror(-wait_ret));
		}
	}

	io_uring_queue_exit(&ring);
	return 0;
}








