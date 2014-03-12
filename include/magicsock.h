#ifndef __MAGICSOCK_H
#define __MAGICSOCK_H

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

#ifndef EXPORT
#define EXPORT
#endif /* EXPORT */

struct ms_handle;
struct ms_client;

typedef void(ms_callback)(struct ms_client *client, void *ctx, const char *addr, int fd);

EXPORT struct ms_handle *ms_start_tcp(int port, void *ctx, ms_callback *callback, int use_threads);
EXPORT struct ms_handle *ms_start_udp(int port, void *ctx, ms_callback *callback, int use_threads);
EXPORT struct ms_handle *ms_start_named(const char *path, void *ctx, ms_callback *callback, int use_threads);

EXPORT int ms_stop(struct ms_handle *socket);

#endif /* __MAGICSOCK_H */
