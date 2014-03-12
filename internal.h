#ifndef INTERNAL_H
#define INTERNAL_H

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

#define LH      fprintf(stderr, "%s:%d %s() LOG HERE!\n", __FILE__, __LINE__, __FUNCTION__);
#define LHP(p)  fprintf(stderr, "%s:%d %s() LOG HERE! - " #p " @ %p\n", __FILE__, __LINE__, __FUNCTION__, p);

#ifdef EXPORT
#undef EXPORT
#endif /* EXPORT */
#define EXPORT __attribute__((visibility("default")))

#include <magicsock.h>
#include <pthread.h>

enum ms_type {
	_MS_TYPE_UNKNOWN_ = 0,
	MS_TYPE_TCP,
	MS_TYPE_UDP,
	MS_TYPE_NAMED,
	_MS_TYPE_EOL_
};

struct ms_client {
	pthread_t tid;
	int fd;

	struct ms_handle *parent;
	struct ms_client *next;

	size_t addrlen;
	char addr[];
};

struct ms_handle {
	enum ms_type type;

	void *ctx;
	ms_callback *callback;
	int use_threads : 1;
	int stopping    : 1;
	int stopped     : 1;

	union {
		int listen_port;
		char *listen_path;
	};

	pthread_t listen_tid;
	int fd;

	pthread_rwlock_t client_lock;
	struct ms_client *clients;
};

void *ms_listen(struct ms_handle *socket);

int _ms_stop(struct ms_handle *socket, int dofree);

#endif /* INTERNAL_H */

