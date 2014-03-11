#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "internal.h"

int ms_stop(struct ms_handle *socket) {
	shutdown(socket->fd, SHUT_RDWR);
	close(socket->fd);

	while (socket->clients != NULL) {
		struct ms_client *c = socket->clients;
		if (pthread_cancel(c->tid) != 0) {
			printf("error killing thread %p\n", (void*)c->tid);
			return -1;
		}
		if (pthread_join(c->tid, NULL) != 0) {
			printf("error joining with thread %p\n", (void*)c->tid);
			return -1;
		}
		shutdown(c->fd, SHUT_RDWR);
		close(c->fd);
		socket->clients = c->next;
		free(c);
	}

	if (socket->type == MS_TYPE_NAMED) free(socket->listen_path);
	free(socket);
	return 0;
}

