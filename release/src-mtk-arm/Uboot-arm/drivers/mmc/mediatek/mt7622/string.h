#ifndef STRING_H
#define STRING_H

#ifdef __cplusplus
extern "C" {
#endif

extern int strlen(const char *s);
extern int strcmp(const char *cs, const char *ct);
extern int strncmp(const char *cs, const char *ct, int count);
extern void *memset(void *s, int c, int count);
extern void *memcpy(void *dest, const void *src, int count);
extern int memcmp(const void *cs, const void *ct, int count);

#ifdef __cplusplus
}
#endif

#endif /* STRING_H */

