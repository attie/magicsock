#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "internal.h"

int ms_stop(struct ms_handle *socket) {
	socket->stopping = 1;

	shutdown(socket->fd, SHUT_RDWR);
	close(socket->fd);

	pthread_rwlock_wrlock(&socket->client_lock);

	while (socket->clients != NULL) {
		struct ms_client *c = socket->clients;
		socket->clients = c->next;

		if (socket->use_threads) {
			if (pthread_cancel(c->tid) != 0) {
				printf("error killing thread %p\n", (void*)c->tid);
				return -1;
			}
		}
		shutdown(c->fd, SHUT_RDWR);
		close(c->fd);
		socket->clients = c->next;
		free(c);
	}

	pthread_rwlock_unlock(&socket->client_lock);
	pthread_rwlock_destroy(&socket->client_lock);

	if (socket->type == MS_TYPE_NAMED) free(socket->listen_path);
	free(socket);
	return 0;
}

