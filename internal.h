#ifndef INTERNAL_H
#define INTERNAL_H

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
	struct ms_client *next;
};

struct ms_handle {
	enum ms_type type;

	void *ctx;
	ms_callback *callback;

	union {
		int listen_port;
		char *listen_path;
	};

	pthread_t listen_tid;
	int fd;

	struct ms_client *clients;
};

void *ms_listen(struct ms_handle *socket);

#endif /* INTERNAL_H */

