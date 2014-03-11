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
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "internal.h"

EXPORT struct ms_handle *ms_start_named(const char *path, void *ctx, ms_callback *callback, int use_threads) {
	size_t l;
	struct ms_handle *h;
	struct sockaddr_un sa;
	char path2[PATH_MAX];

	realpath(path, path2);
	if ((l = strlen(path2)) >= sizeof(sa.sun_path)) return NULL;

	if ((h = malloc(sizeof(*h))) == NULL) return NULL;
	memset(h, 0, sizeof(h));
	h->type = MS_TYPE_NAMED;
	h->ctx = ctx;
	h->callback = callback;
	h->use_threads = use_threads;
	h->listen_path = strdup(path2);

	if (pthread_rwlock_init(&h->client_lock, NULL) != 0) goto die0;

	if ((h->fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) goto die1;

	memset(&sa, 0, sizeof(sa));
	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, h->listen_path);

	if (bind(h->fd, (const struct sockaddr *)&sa, sizeof(sa.sun_family) + l) != 0) goto die2;

	if (listen(h->fd, 512) != 0) goto die2;

	if (pthread_create(&h->listen_tid, NULL, (void*(*)(void*))ms_listen, h) != 0) goto die2;

	return h;

die2:
	shutdown(h->fd, SHUT_RDWR);
	close(h->fd);
die1:
	pthread_rwlock_destroy(&h->client_lock);
die0:
	free(h->listen_path);
	free(h);
	return NULL;
}

