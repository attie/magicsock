#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "internal.h"

void *ms_listen(struct ms_handle *socket) {
	shutdown(socket->fd, SHUT_RDWR);
	close(socket->fd);
	if (socket->type == MS_TYPE_NAMED) free(socket->listen_path);
	free(socket);
	pthread_detach(pthread_self());
	return NULL;
}
