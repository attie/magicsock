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
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "internal.h"

void *ms_client(struct ms_client *client) {
	struct ms_handle *socket;
	struct ms_client **c;

	socket = client->parent;
	
	socket->callback(client, socket->ctx, client->addr, client->fd);

	if (socket->use_threads) pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	/* tidy up */
	pthread_rwlock_wrlock(&socket->client_lock);
	for (c = &(socket->clients); *c && *c != client; c = &((*c)->next));
	if (*c) { /* this should always be true... otherwise we failed to find ourself */
		/* unlink this client from the chain */
		*c = client->next;
	}
	client->next = NULL;
	pthread_rwlock_unlock(&socket->client_lock);
	
	/* destroy this client */
	shutdown(client->fd, SHUT_RDWR);
	close(client->fd);
	free(client);

	return NULL;
}

void ms_client_prepare(struct ms_handle *socket, const char *addr, size_t addrlen, int fd) {
	struct ms_client *client, **c;
	size_t clientlen;

	clientlen = sizeof(*client) + (sizeof(char) * (addrlen + 1));

	if ((client = malloc(clientlen)) == NULL) return;
	memset(client, 0, clientlen);
	client->fd = fd;
	client->parent = socket;
	client->addrlen = addrlen;
	strncpy(client->addr, addr, addrlen);
	client->addr[addrlen] = '\0';

	pthread_rwlock_wrlock(&socket->client_lock);
	for (c = &(socket->clients); *c; c = &((*c)->next));
	*c = client;
	pthread_rwlock_unlock(&socket->client_lock);

	if (socket->use_threads && pthread_create(&client->tid, NULL, (void*(*)(void*))ms_client, client) == 0) {
		/* all threads are created and immediately detached */
		pthread_detach(client->tid);
	} else { /* if not using threads, or creation of thread failed... */
		ms_client(client);
	}
}

void *ms_listen(struct ms_handle *socket) {
	while (1) {
		uint8_t buf[2048];
		size_t buflen;
		int fd;

		char addr[2048];
		size_t addrlen;

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		buflen = sizeof(buf);
		if ((fd = accept(socket->fd, (void*)buf, &buflen)) == -1) {
			/* uhoh... */
			goto die;
		}
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

		/* convert the address to ASCII */
		switch (socket->type) {
			case MS_TYPE_TCP:
			case MS_TYPE_UDP: {
				struct sockaddr_storage *a = (struct sockaddr_storage*)buf;
				int port;
				if (a->ss_family == AF_INET) {
					struct sockaddr_in *s = (struct sockaddr_in*)a;
					port = ntohs(s->sin_port);
					if (inet_ntop(AF_INET, &(s->sin_addr), addr, sizeof(addr)) == NULL) {
						strcpy(addr, "unknown");
					}
				} else if (a->ss_family == AF_INET6) {
					struct sockaddr_in6 *s = (struct sockaddr_in6*)a;
					port = ntohs(s->sin6_port);
					if (inet_ntop(AF_INET6, &(s->sin6_addr), addr, sizeof(addr)) == NULL) {
						strcpy(addr, "unknown");
					}
				}
				addrlen = strlen(addr);
				snprintf(&(addr[addrlen]), sizeof(addr) - addrlen, ":%d", port);
				break;
			}
			case MS_TYPE_NAMED: {
				strcpy(addr, "unix_socket");
				break;
			}	
			default:
				/* this shouldn't happen */
				goto die;
		}

		addrlen = strlen(addr);
		ms_client_prepare(socket, addr, addrlen, fd);
	}

	/* shouldn't really get here...... */

die:
	if (!socket->stopping) _ms_stop(socket, 0);
	return NULL;
}
