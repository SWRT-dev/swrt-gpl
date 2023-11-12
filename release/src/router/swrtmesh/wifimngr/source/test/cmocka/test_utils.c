#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <uci.h>

#include <json-c/json.h>

#include "test_utils.h"

char *multi_tok(char *input, char *delimiter)
{
    static char *string;
	char *end, *temp;

    if (input)
        string = input;

    if (!string)
        return string;

    end = strstr(string, delimiter);

    if (!end) {
        char *t = string;

        string = NULL;
        return t;
    }

    temp = string;

    *end = '\0';
    string = end + strlen(delimiter);

    return temp;
}

/* TODO: how to fix events without fopen and fclose every poll? */
struct json_object *poll_test_log(FILE *fp, const char *prefix)
{
	char line[256] = {0};
	char msg[256] = {0};
	struct json_object *obj = NULL;

	fp = fopen("/tmp/test.log", "r");
	if (!fp)
		return NULL;

	while (fgets(line, 256, fp))  {
		char *ptr, *s;
		char appended[32];

		snprintf(appended, sizeof(appended), "%s:", prefix);

		ptr = strstr(line, appended);
		if (!ptr)
			continue;

		multi_tok(ptr, appended);
		s = multi_tok(NULL, appended);
		strncpy(msg, s, sizeof(msg));
	}

	if (strlen(msg))
		obj = json_tokener_parse(msg);

	fclose(fp);
	return obj;
}

struct uci_package *init_package(struct uci_context **ctx, const char *config)
{
	struct uci_package *p = NULL;

	if (*ctx == NULL) {
		*ctx = uci_alloc_context();
	} else {
		p = uci_lookup_package(*ctx, config);
		if (p)
			uci_unload(*ctx, p);
	}

	if (uci_load(*ctx, config, &p))
		return NULL;

	return p;
}

int cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}