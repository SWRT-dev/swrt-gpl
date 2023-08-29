#define _GNU_SOURCE /* for getopt_long() */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "roxml.h"

static struct option long_options[] =
{
	{ "help", 0, NULL, 'h' },	/* help/usage */
	{ "quiet", 0, NULL, 'q' },	/* help/usage */
	{ 0, 0, 0, 0 }
};

static const char* short_options = "-hq";

static void display_usage(const char* prgname)
{
	char *name = basename(prgname);
	
	fprintf(stdout, "%s [options] <FILE> [<XPATH>]\n", name);
	fprintf(stdout, "\tShow a specific XML subtree from a file\n");
	fprintf(stdout, "options:    --help|-h                display help/usage\n");
	fprintf(stdout, "options:    --quiet|-q               don't output errors\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Notes\n");
	fprintf(stdout, "\tthe XPATH argument is OPTIONAL. If it's not specified, the function\n");
	fprintf(stdout, "\tshows the complete XML tree ; otherwise, it shows only the selected\n");
	fprintf(stdout, "\tpart of the tree.\n");
	fprintf(stdout, "\n");
}

#define ROCAT_FLAG_QUIET		0x00000001

int main(int ac, char *av[])
{
	node_t 			*doc;
	char			*xmlname = NULL;
	char			*xpquery = NULL;
	node_t 			**xpr = NULL;
	int 			opt = 0;
	int 			longopt = 0;
	int			flags = 0;

	/* getopt_long() should not display any message */
	opterr = 0; 

	while ((opt = getopt_long(ac, av, short_options, long_options, &longopt)) != -1) {
		switch (opt) {
		case 0:
			/*
			long option encountered
			we should not see any of them, because they are supposed to
			map to the corresponding short option. So if we get this, it
			might indicate that something is borked.
			*/
			break;
		case 1:
			/* not an option */
			if (!xmlname) {
				xmlname = strdup(av[optind-1]);
			} else if (!xpquery) {
				xpquery = strdup(av[optind-1]);
			} else {
				fprintf(stderr, "too many non-option - remove '%s'\n", av[optind - 1]);
				return EXIT_FAILURE;
			}
			break;
		case 'h':
			display_usage(av[0]);
			return EXIT_FAILURE;
		case 'q':
			flags |= ROCAT_FLAG_QUIET;
			break;
		case '?':
			fprintf(stderr, "unknown option '%s'\n", av[optind - 1]);
			return EXIT_FAILURE;
		default:
			break;
		}
	}

	if (!xmlname) {
		if ((flags & ROCAT_FLAG_QUIET) == 0) {
			fprintf(stderr, "invalid options: no XML file name\n\n");
			display_usage(av[0]);
		}
		return EXIT_FAILURE;
	}

	doc = roxml_load_doc(xmlname);
	if (!doc) {
		if ((flags & ROCAT_FLAG_QUIET) == 0)
			fprintf(stderr, "failed to load XML file '%s'\n", xmlname);
		goto on_error;
	}

	if (xpquery) {
		int xncount;

		xpr = roxml_xpath(doc, xpquery, &xncount);
		if (!xpr) {
			if ((flags & ROCAT_FLAG_QUIET) == 0)
				fprintf(stderr, "XPath query '%s' failed on '%s'\n", 
					xpquery, xmlname);
			goto on_error;
		}

		if (xncount) {
			int npos; 
			
			for (npos = 0; npos < xncount; ++npos) {
				char *buffer = NULL;
				node_t *xn = xpr[npos];;
				/* commit the node to stdout */
				roxml_commit_changes(xn, NULL, &buffer, 1);
				printf("%s\n", buffer);
				free(buffer);
			}
		} else {
			if ((flags & ROCAT_FLAG_QUIET) == 0)
				fprintf(stderr, "XPath query '%s' does not yield to any result\n", 
					xpquery);
			goto on_error;
		}

		roxml_release(xpr);

		free(xpquery);
	} else {
		char *buffer = NULL;
		/* commit everything to stdout */
		roxml_commit_changes(doc, NULL, &buffer, 1);
		printf("%s\n", buffer);
		free(buffer);
	}

	roxml_close(doc);

	free(xmlname);

	return EXIT_SUCCESS;

on_error:
	if (xpr) {
		roxml_release(xpr);
	}
	if (doc) roxml_close(doc);
	if (xmlname) free(xmlname);
	if (xpquery) free(xpquery);

	return EXIT_FAILURE;
}
