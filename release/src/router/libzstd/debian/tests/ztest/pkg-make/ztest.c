#include <stdio.h>

#include <zstd.h>

int
main(void)
{
	const unsigned ver = ZSTD_versionNumber();
	printf("Found libzstd %u.%u.%u\n",
	    (ver / 10000), (ver / 100) % 100, ver % 100);
	return 0;
}
