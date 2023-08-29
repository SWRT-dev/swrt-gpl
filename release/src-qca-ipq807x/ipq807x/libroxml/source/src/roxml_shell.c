/**
 * \file roxml_shell.c
 * \brief shell xpath solver sources
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include "roxml.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(void)
{
	fprintf(stderr, "LGPL command line XML parser\n<tristan.lelong@libroxml.net>\n");
}

void print_usage(const char *progname)
{
	fprintf(stderr, "\nusage: %s [-q|-h] <filename> [/]<node1>/<node2>/<node3>/.../<nodeN>\n", progname);
	fprintf(stderr, "-q|--quiet: activate quiet mode\n");
	fprintf(stderr, "-c|--content: show content of node instead of name\n");
	fprintf(stderr, "-h|--help: display this message\n");
}

int main(int argc, char **argv)
{
	int optind;
	int quiet = 0;
	int content = 0;
	int j, max;
	node_t *root;
	node_t **ans;

	for (optind = 1; optind < argc; optind++) {
		int option = 0;
		if (argv[optind][0] == '-') {
			if (strcmp(argv[optind], "--help") == 0) {
				option = 'h';
			} else if (strcmp(argv[optind], "--content") == 0) {
				option = 'c';
			} else if (strcmp(argv[optind], "--quiet") == 0) {
				option = 'q';
			} else if (strcmp(argv[optind], "-h") == 0) {
				option = 'h';
			} else if (strcmp(argv[optind], "-c") == 0) {
				option = 'c';
			} else if (strcmp(argv[optind], "-q") == 0) {
				option = 'q';
			}
		} else {
			break;
		}
		switch (option) {
		case 'q':
			quiet = 1;
			break;
		case 'c':
			content = 1;
			break;
		case 'h':
			print_help();
			print_usage(argv[0]);
			return EXIT_FAILURE;
			break;
		default:
			print_usage(argv[0]);
			return EXIT_FAILURE;
			break;
		}
	}

	root = roxml_load_doc(argv[optind]);
	if (root == NULL) {
		if (!quiet)
			perror("error parsing xml file");
		goto error;
	}

	if (argc < optind + 2)
		ans = roxml_xpath(root, "/", &max);
	else
		ans = roxml_xpath(root, argv[optind + 1], &max);

	for (j = 0; j < max; j++) {
		char *c = NULL;

		// Dump the XML subtree.
		if (content) {
			roxml_commit_fd(ans[j], 1, 1);
			fprintf(stdout, "\n");
			fflush(stdout);
			continue;
		}

		// Or display:
		// - the node content for leaf nodes.
		// - list of children otherwise. 
		c = roxml_get_content(ans[j], NULL, 0, NULL);
		if (c && *c) {
			if (!quiet)
				fprintf(stdout, "ans[%d]: ", j);
			fprintf(stdout, "%s\n", c);
		} else if (c) {
			char *s = NULL;
			int nb_chld = roxml_get_chld_nb(ans[j]);
			int i = 0;

			for (i = 0; i < nb_chld; i++) {
				node_t *child = roxml_get_chld(ans[j], NULL, i);
				node_t *ns = roxml_get_ns(child);
				c = roxml_get_name(child, NULL, 0);
				if (!quiet)
					fprintf(stdout, "ans[%d]: ", j);
				if (ns) {
					s = roxml_get_name(ns, NULL, 0);
					if (strlen(s))
						fprintf(stdout, "%s:", s);
				}
				fprintf(stdout, "%s\n", c);
			}
		}
	}
	roxml_release(RELEASE_ALL);

	roxml_close(root);

error:
	return 0;
}
