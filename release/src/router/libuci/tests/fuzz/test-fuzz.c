#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "uci.h"

static void fuzz_uci_import(const uint8_t *input, size_t size)
{
	int r;
	int fd;
	FILE *fs = NULL;
	struct uci_context *ctx = NULL;
	struct uci_package *package = NULL;

	fd = open("/dev/shm", O_TMPFILE | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		perror("unable to create temp file");
		exit(-1);
	}

	r = write(fd, input, size);
	if (r < 0) {
		perror("unable to write()");
		exit(-1);
	}

	fs = fdopen(fd, "r");
	if (fs == NULL) {
		perror("unable to fdopen()");
		exit(-1);
	}

	fseek(fs, 0L, SEEK_SET);

	ctx = uci_alloc_context();
	if (ctx == NULL) {
		perror("unable to uci_alloc_context()");
		exit(-1);
	}

	uci_import(ctx, fs, NULL, &package, false);
	uci_free_context(ctx);
	close(fd);
	fclose(fs);
}

int LLVMFuzzerTestOneInput(const uint8_t *input, size_t size)
{
	fuzz_uci_import(input, size);
	return 0;
}
