#include <common.h>
#include <command.h>

static int do_dialog(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	int ch;
	int putnl = 1;

	for (i = 1; i < argc; i++) {
		char *p = argv[i];
		char *nls; /* new-line suppression */

		if (i > 1)
			putc(' ');

		nls = strstr(p, "\\c");
		if (nls) {
			char *prenls = p;

			putnl = 0;
			/*
			 * be paranoid and guess that someone might
			 * say \c more than once
			 */
			while (nls) {
				*nls = '\0';
				puts(prenls);
				*nls = '\\';
				prenls = nls + 2;
				nls = strstr(prenls, "\\c");
			}
			puts(prenls);
		} else {
			puts(p);
		}
	}

	if (putnl)
		putc('\n');

	puts("Continue (Y/N):");
	ch = getc();
	putc(ch);
	putc('\n');
	if ((ch == 'y') || (ch == 'Y'))
		return 0;
	else
		return 1;
}

U_BOOT_CMD(
	dialog,	CONFIG_SYS_MAXARGS,	1,	do_dialog,
	"echo args to console, and get yes or no response from user",
	"[args..]\n"
	"    - echo args to console, and get yes or no; \\c suppresses newline"
);
