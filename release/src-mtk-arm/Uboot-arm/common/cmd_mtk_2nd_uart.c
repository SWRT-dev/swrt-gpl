#include <common.h>
#include <serial.h>
#include <command.h>

extern volatile unsigned int g_uart;
extern volatile unsigned int g_2nd_uart;
extern struct serial_device *default_serial_console(void);
extern struct serial_device *mtk_2nd_serial_console(void);

static int mtk_2nd_uart_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int c,i;
	printf("---------- test default uart -------------\n");
	default_serial_console()->start();
	default_serial_console()->setbrg();
	printf("\n");
	printf("default uart base: %x\n", g_uart);
	printf("output 'Hello World' on %s\n", default_serial_console()->name);
	default_serial_console()->puts("Hello World!\n");


	printf("---------- test second uart -------------\n");
	mtk_2nd_serial_console()->start();
	mtk_2nd_serial_console()->setbrg();
	printf("second uart base: %x\n", g_2nd_uart);
	printf("output 'Hello World' on %s\n", mtk_2nd_serial_console()->name);
	mtk_2nd_serial_console()->puts("Hello World!\n");
	printf("please input anything from %s\n", mtk_2nd_serial_console()->name);
	for (i=0 ; i<5 ; i++) {
		c = mtk_2nd_serial_console()->getc();
		printf("%s> %c\n", mtk_2nd_serial_console()->name, c);
	}

	return 0;
}

U_BOOT_CMD(mtk_2nd_uart_test, 1, 0, mtk_2nd_uart_test,
	"mtk_2nd_uart_test",
	"mtk_2nd_uart_test\n"
);
