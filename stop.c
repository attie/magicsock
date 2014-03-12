/*
	magicsock - a magical socket library

	Copyright (C) 2014 onwards  Attie Grande (attie@attie.co.uk)

	magicsock is free software: you can redistribute it and/or modify it
	under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	magicsock is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with magicsock. If not, see <http://www.gnu.org/licenses/>.
*/

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
#warning TODO - this may require some re-work
				printf("magicsock: error killing thread %p\n", (void*)c->tid);
				continue;
			}
		}
		shutdown(c->fd, SHUT_RDWR);
		close(c->fd);
		socket->clients = c->next;
		free(c);
	}

	pthread_rwlock_unlock(&socket->client_lock);
	pthread_rwlock_destroy(&socket->client_lock);

	if (socket->type == MS_TYPE_NAMED) {
		unlink(socket->listen_path);
		free(socket->listen_path);
	}
	free(socket);
	return 0;
}

