#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <magicsock.h>

void my_cb(void *ctx, const char *addr, int fd) {
	printf("connection from '%s' on fd %d\n", addr, fd);
	sleep(2);
	printf("fd %d finished!\n", fd);
}

int main(int argc, char *argv[]) {
	struct ms_handle *s;
	int ret;

	if ((s = ms_start_tcp(27015, "context info", my_cb, 1)) == NULL) {
		printf("ms_start_tcp() failed...\n");
		return -1;
	}

	sleep(10);

	if ((ret = ms_stop(s)) != 0) {
		printf("ms_stop() failed... returned %d\n", ret);
	}

	return 0;
}
