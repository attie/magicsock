#ifndef __MAGICSOCK_H
#define __MAGICSOCK_H

#ifndef EXPORT
#define EXPORT
#endif /* EXPORT */

struct ms_handle;
typedef void(ms_callback)(void *ctx, const char *addr, int fd);

EXPORT struct ms_handle *ms_start_tcp(int port, void *ctx, ms_callback *callback, int use_threads);
EXPORT struct ms_handle *ms_start_udp(int port, void *ctx, ms_callback *callback, int use_threads);
EXPORT struct ms_handle *ms_start_named(const char *path, void *ctx, ms_callback *callback, int use_threads);

EXPORT int ms_stop(struct ms_handle *socket);

#endif /* __MAGICSOCK_H */
