#include "simple_uring.h"


int setup_context(unsigned queue_depth, struct io_uring *ring){
        int ret = io_uring_queue_init(queue_depth, ring, 0);
        if (ret < 0) {
                fprintf(stderr, "queue_init: %s\n", strerror(-ret));
                return -1;
        }

        return 0;
}

int queue_write(struct io_uring *ring, struct io_data *data, int fd){
        struct io_uring_sqe *sqe;
        sqe = io_uring_get_sqe(ring);
        assert(sqe);

	struct io_uring_cqe *cqe;
	
	io_uring_prep_writev(sqe, fd, &data->iov, 1, data->offset);
        io_uring_sqe_set_data(sqe, data);
	
	int pending;
	pending = io_uring_submit(ring);

	return pending;
}

int m_write(char *src, int len, int writes, int offset, int fd){
	struct io_uring ring;
	struct io_data *data = malloc(sizeof(struct io_data));

	struct io_uring_cqe *cqe;
	struct io_uring_sqe *sqe;

	if(setup_context(QD, &ring) != 0){
		return 1;
	}

	data->first_offset = 0;
	data->iov.iov_base = src;
	data->iov.iov_len = len;
	data->offset = data->offset;
	
	static int count = 0;
	int pending;
	int wait_ret;

	while(count < writes){
		memcpy(data->iov.iov_base, src, 2);
		pending = queue_write(&ring, data, fd);

		count++;
		data->offset = count;
	}
	
	if (pending < 0){
		fprintf(stderr, "io_uring_submit: %s\n", strerror(-pending));
		return 1;
	}
	
	for(int i = 0; i < pending; i++){
		wait_ret = io_uring_wait_cqe(&ring, &cqe);
		if (wait_ret < 0){
			fprintf(stderr, "io_uring_wait_cqe: %s\n", strerror(-wait_ret));
		}
	}
	
	io_uring_queue_exit(&ring);
	return 0;
}








