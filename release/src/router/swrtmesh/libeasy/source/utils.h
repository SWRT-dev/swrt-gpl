#ifndef EASYUTILS_H
#define EASYUTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Convert hex string to byte array
 *
 * For example:
 * "001122334455" --> {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}
 *
 * @param[in] str hex string.
 * @param[in] len length of output buffer passed to the function.
 * @param[out] bytes output buffer to write the converted hex string.
 * @return byte array of the converted hex string, or %NULL on error.
 */
extern uint8_t *strtob(char *str, int len, uint8_t *bytes);


/** Convert byte array to hex string
 *
 * For example:
 * {0x00, 0x11, 0x22, 0x33, 0x44, 0x55} --> "001122334455"
 *
 * @param[in] bytes byte array.
 * @param[in] len length of the byte array.
 * @param[out] str output buffer to write the hex string.
 * @return hex string or %NULL on error.
 */
extern char *btostr(const uint8_t *bytes, int len, char *str);


/** Convert macaddress from ':' separated string to byte array
 *
 * For example:
 * "00:11:22:33:44:55" --> {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}
 *
 * @param[in] macstr mac address string.
 * @param[out] mac output buffer to write the converted macstr.
 * @return output byte array of the converted macstr.
 */
extern uint8_t *hwaddr_aton(const char *macstr, uint8_t *mac);


/** Convert macaddress from byte array to ':' separated string
 *
 * For example:
 * {0x00, 0x11, 0x22, 0x33, 0x44, 0x55} --> "00:11:22:33:44:55"
 *
 * @param[in] mac macaddress byte array.
 * @param[out] macstr mac address string in ':' sepearted format.
 * @return output macaddress string in ':' separated format.
 */
extern char *hwaddr_ntoa(const uint8_t *mac, char *macstr);

/** Convert UUID hex string to byte array.
 *
 * For example:
 * "01234567-89ab-cdef-0123-456789abcdef" -->
 *	{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, ..., 0xef}
 *
 * @param[in] uuidstr hex string in specified format.
 * @param[out] uuid output in uuid 16 bytes buffer.
 * @return 0 on success, -1 on error.
 */
extern int uuid_strtob(char *uuidstr, uint8_t *uuid);

/** Convert UUID byte array to hex string.
 *
 * For example:
 * {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,..., 0xef} -->
 *	"01234567-89ab-cdef-0123-456789abcdef"
 *
 * @param[in] uuid bytes array.
 * @param[out] uuidstr output in hex-string.
 * @return 0 on success, -1 on error.
 */
extern int uuid_btostr(uint8_t *uuid, char *uuidstr);

static inline int hwaddr_equal(const uint8_t *a, const uint8_t *b)
{
	return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2])
		| (a[3] ^ b[3]) | (a[4] ^ b[4]) | (a[5] ^ b[5])) == 0;
}

static inline bool hwaddr_is_zero(const uint8_t *a)
{
	return (a[0] | a[1] | a[2] | a[3] | a[4] | a[5]) == 0;
}

static inline bool hwaddr_is_bcast(const uint8_t *a)
{
	return (a[0] & a[1] & a[2] & a[3] & a[4] & a[5]) == 0xff;
}

static inline bool hwaddr_is_ucast(const uint8_t *a)
{
	return !(a[0] & 0x01);
}

static inline bool hwaddr_is_mcast(const uint8_t *a)
{
	return !!(a[0] & 0x01);
}

#define MACFMT		"%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(_m)	(_m)[0], (_m)[1], (_m)[2], (_m)[3], (_m)[4], (_m)[5]
#define MACSTR MACFMT


/** IFF_* device flags */
typedef unsigned int ifstatus_t;

/** IF_OPER_* operstate flags */
typedef unsigned char ifopstatus_t;

/* To be deprecated; use if_setflags() and if_getoperstate() instead */
extern int get_ifstatus(const char *ifname, ifstatus_t *f);
extern int get_ifoperstatus(const char *ifname, ifopstatus_t *opstatus);


#define UNUSED(var)	do { (void)(var); } while(0)
#define UNUSED_FUNC	__attribute__((unused)) 

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))
#endif

#ifndef offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif

#ifndef bit
#define bit(_n)	(1 << (_n))
#endif

#ifndef min
#define min(x, y)	(x) < (y) ? (x) : (y)
#endif

#ifndef max
#define max(x, y)	(x) < (y) ? (y) : (x)
#endif


/* install a signal handler */
extern int set_sighandler(int sig, void (*handler)(int));

/* uninstall a signal handler */
extern int unset_sighandler(int sig);



extern char *trim(char *str);
extern void remove_newline(char *buf);
extern int runCmd(const char *format, ...);
extern char *chrCmd(char *output, size_t output_size, const char *format, ...);
extern int Cmd(char *output, size_t output_size, const char *format, ...);

struct getopt_option {
	int argc;
	int next;
	char *value;
};

extern int getopt_r(int argc, char * const argv[], const char *optstring,
		    struct getopt_option *opt);

#if defined(RTCONFIG_SWRTMESH)
extern int easy_base64_encode(const unsigned char *src, size_t len,
                               unsigned char *out, size_t *out_len);
extern int easy_base64_decode(const unsigned char *src, size_t len,
                               unsigned char *out, size_t *out_len);
#else
extern int base64_encode(const unsigned char *src, size_t len,
				unsigned char *out, size_t *out_len);
extern int base64_decode(const unsigned char *src, size_t len,
				unsigned char *out, size_t *out_len);
#endif

extern int base64url_encode(uint8_t *out, uint8_t *data, size_t len);
extern int base64url_decode(uint8_t *out, uint8_t *data, int len);

#ifdef __cplusplus
}
#endif

#endif /* EASYUTILS_H */
