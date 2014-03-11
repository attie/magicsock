#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "internal.h"

EXPORT struct ms_handle *ms_start_tcp(int port, void *ctx, ms_callback *callback) {
	struct ms_handle *h;
	struct sockaddr_in sa;

	if ((h = malloc(sizeof(*h))) == NULL) return NULL;
	memset(h, 0, sizeof(h));
	h->type = MS_TYPE_TCP;
	h->ctx = ctx;
	h->callback = callback;
	h->listen_port = port;

	if ((h->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) goto die;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(h->listen_port);
	sa.sin_addr.s_addr = INADDR_ANY;

	if (bind(h->fd, (const struct sockaddr *)&sa, sizeof(sa)) != 0) goto die;

	if (listen(h->fd, 512) != 0) goto die;

	if (pthread_create(&h->listen_tid, NULL, (void*(*)(void*))ms_listen, h) != 0) goto die;

	return h;
die:
	free(h);
	return NULL;
}
