#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "internal.h"

EXPORT struct ms_handle *ms_start_named(const char *path, void *ctx, ms_callback *callback) {
	size_t l;
	struct ms_handle *h;
	struct sockaddr_un sa;

	if ((l = strlen(path)) >= sizeof(sa.sun_path)) return NULL;

	if ((h = malloc(sizeof(*h))) == NULL) return NULL;
	memset(h, 0, sizeof(h));
	h->type = MS_TYPE_NAMED;
	h->ctx = ctx;
	h->callback = callback;
	h->listen_path = strdup(path);

	if ((h->fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) goto die;

	memset(&sa, 0, sizeof(sa));
	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, h->listen_path);

	if (bind(h->fd, (const struct sockaddr *)&sa, sizeof(sa.sun_family) + l) != 0) goto die;

	if (listen(h->fd, 512) != 0) goto die;

	if (pthread_create(&h->listen_tid, NULL, (void*(*)(void*))ms_listen, h) != 0) goto die;

	return h;
die:
	free(h->listen_path);
	free(h);
	return NULL;
}

