#include <sys/time.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int cpu_only;
static int tsc_only;
static int use_ints;

/* returns current time in microseconds */
static inline unsigned long long microseconds(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000ULL + tv.tv_usec;
}

#if defined(__i386__) || defined(__x86_64__)
#define HAVE_RDTSC 1
#endif

#ifdef HAVE_RDTSC
static inline unsigned long long rdtsc(void)
{
	unsigned int a, d;
	asm volatile("rdtsc" : "=a" (a), "=d" (d));
	return a + ((unsigned long long)d << 32);
}
#else
#define rdtsc() 0
#endif

/* performs read-after-write operations that the CPU is not supposed to be able
 * to parallelize. The "asm" statements are here to prevent the compiler from
 * reordering this code.
 */
#define dont_move(var) do { asm volatile("" : "=r"(var) : "0" (var)); } while (0)

#define run1cycle_ae()   do { a ^= e; dont_move(a); } while (0)
#define run1cycle_ba()   do { b ^= a; dont_move(b); } while (0)
#define run1cycle_cb()   do { c ^= b; dont_move(c); } while (0)
#define run1cycle_dc()   do { d ^= c; dont_move(d); } while (0)
#define run1cycle_ed()   do { e ^= d; dont_move(e); } while (0)
#define run1cycle_eb()   do { e ^= b; dont_move(e); } while (0)

#define run5cycles()                                    \
	do {                                            \
		run1cycle_ae();				\
		run1cycle_ba();				\
		run1cycle_cb();				\
		run1cycle_dc();				\
		run1cycle_ed();				\
	} while (0)

#define run10cycles()          \
	do {                   \
		run5cycles();  \
		run5cycles();  \
	} while (0)

#define run100cycles()          \
	do {                    \
		run10cycles();  \
		run10cycles();  \
		run10cycles();  \
		run10cycles();  \
		run10cycles();  \
		run10cycles();  \
		run10cycles();  \
		run10cycles();  \
		run10cycles();  \
		run10cycles();  \
	} while (0)


/* performs 50 operations in a loop, all dependant on each other, so that the
 * CPU cannot parallelize them, hoping to take 50 cycles per loop, plus the
 * loop counter overhead.
 */
static __attribute__((noinline,aligned(64))) void loop50(unsigned int n)
{
	unsigned int a = 0, b = 0, c = 0, d = 0, e = 0;

	do {
		run10cycles();
		run10cycles();
		run10cycles();
		run10cycles();
		run10cycles();
	} while (__builtin_expect(--n, 1));
}

/* performs 250 operations in a loop, all dependant on each other, so that the
 * CPU cannot parallelize them, hoping to take 250 cycles per loop, plus the
 * loop counter overhead. Do not increase this loop so that it fits in a small
 * 1 kB L1 cache on 32-bit instruction sets.
 */
static __attribute__((noinline,aligned(64))) void loop250(unsigned int n)
{
	unsigned int a = 0, b = 0, c = 0, d = 0, e = 0;

	do {
		run10cycles();
		run10cycles();
		run10cycles();
		run10cycles();
		run10cycles();
		run100cycles();
		run100cycles();
	} while (__builtin_expect(--n, 1));
}

void run_once(long count)
{
	long long tsc_begin;
	long long tsc_duration50 __attribute__((unused));
	long long tsc_duration250 __attribute__((unused));
	long long us_begin, us_duration50, us_duration250;
	long long us_duration;
	unsigned int i;
	char mhz[20];

	/* now run the 50 cycles loop. We'll pick the lowest value
	 * among 5 runs of the short loop.
	 */
	us_duration50 = LLONG_MAX;
	for (i = 0; i < 5; i++) {
		us_begin   = microseconds();
		tsc_begin  = rdtsc();
		loop50(count);
		tsc_duration50 = rdtsc() - tsc_begin;
		us_duration    = microseconds() - us_begin;
		if (us_duration < us_duration50)
			us_duration50 = us_duration;
	}

	/* now run the 250 cycles loop. We'll pick the lowest value
	 * among 5 runs of the long loop.
	 */
	us_duration250 = LLONG_MAX;
	for (i = 0; i < 5; i++) {
		us_begin   = microseconds();
		tsc_begin  = rdtsc();
		loop250(count);
		tsc_duration250 = rdtsc() - tsc_begin;
		us_duration     = microseconds() - us_begin;
		if (us_duration < us_duration250)
			us_duration250 = us_duration;
	}

	if (use_ints)
		snprintf(mhz, sizeof(mhz), "%.0f", count * 200.0 / (us_duration250 - us_duration50) + 0.5);
	else
		snprintf(mhz, sizeof(mhz), "%.3f", count * 200.0 / (us_duration250 - us_duration50));

	if (!cpu_only && !tsc_only) {
		printf("count=%ld us50=%lld us250=%lld diff=%lld cpu_MHz=%s",
		       count, us_duration50, us_duration250, us_duration250 - us_duration50,
		       mhz);
	}
	else if (cpu_only) {
		printf("%s\n", mhz);
		return;
	}

#ifdef HAVE_RDTSC
	if (use_ints)
		snprintf(mhz, sizeof(mhz), "%.0f", (tsc_duration250 - tsc_duration50) / (float)(us_duration250 - us_duration50) + 0.5);
	else
		snprintf(mhz, sizeof(mhz), "%.3f", (tsc_duration250 - tsc_duration50) / (float)(us_duration250 - us_duration50));

	if (!tsc_only) {
		printf(" tsc50=%lld tsc250=%lld diff=%lld rdtsc_MHz=%s",
		       tsc_duration50, tsc_duration250, (tsc_duration250 - tsc_duration50) / count,
		       mhz);
	} else {
		printf("%s\n", mhz);
		return;
	}
#endif
	putchar('\n');
}

/* spend <delay> us waiting for the CPU's frequency to raise. Will also stop
 * on backwards time jumps if any.
 */
void pre_heat(long delay)
{
	unsigned long long start = microseconds();

	while (microseconds() - start < (unsigned long long)delay)
		;
}

/* determines how long loop50() must be run to reach more than 20 milliseconds.
 * This will ensure that an integral number of clock ticks will have happened
 * on 100, 250, 1000 Hz systems.
 */
unsigned int calibrate(void)
{
	unsigned long long duration = 0;
	unsigned long long start;
	unsigned int count = 1000;

	while (duration < 10000) {
		count = count * 5 / 4;
		start = microseconds();
		loop50(count);
		duration = microseconds() - start;
	}
	return (count * 20000ULL + duration / 2) / duration;
}

void usage(const char *name)
{
	printf("Usage: %s [-h|-c%s]* [lines [heat [count]]]\n"
	       "  -h      show this help\n"
	       "  -c      show CPU freq only (in MHz)\n"
	       "  -i      report integral frequencies only\n"
#ifdef HAVE_RDTSC
	       "  -t      show TSC freq only (in MHz)\n"
#endif
	       "  lines   number of measurements (one line per measurement). Def: 1\n"
	       "  heat    pre-heat time in microseconds. Def: 0\n"
	       "  count   calibration value, higher is slower but more accurate. Def: auto\n"
	       "\n", name,
#ifdef HAVE_RDTSC
	       "|-t"
#else
	       ""
#endif
	       );
	exit(0);
}

int main(int argc, char **argv)
{
	const char *name = argv[0];
	unsigned int count;
	long runs = 1;

	while (argc > 1 && *argv[1] == '-') {
		if (argv[1][1] == 'c')
			cpu_only = 1;
		else if (argv[1][1] == 'i')
			use_ints = 1;
#ifdef HAVE_RDTSC
		else if (argv[1][1] == 't')
			tsc_only = 1;
#endif
		else
			usage(name);
		argc--; argv++;
	}

	if (argc > 1)
		runs = atol(argv[1]);

	if (argc > 2)
		pre_heat(atol(argv[2]));

	count = calibrate();

	if (argc > 3)
		count = atol(argv[3]);

	while (runs--)
		run_once(count);

	return 0;
}

