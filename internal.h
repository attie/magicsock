#ifndef INTERNAL_H
#define INTERNAL_H

#define LH      fprintf(stderr, "%s:%d %s() LOG HERE!\n", __FILE__, __LINE__, __FUNCTION__);
#define LHP(p)  fprintf(stderr, "%s:%d %s() LOG HERE! - " #p " @ %p\n", __FILE__, __LINE__, __FUNCTION__, p);

#ifdef EXPORT
#undef EXPORT
#endif /* EXPORT */
#define EXPORT __attribute__((visibility("default")))

#endif /* INTERNAL_H */

