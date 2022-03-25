#include <common.h>
#include <serial.h>
#include <command.h>
#include <dm.h>
#include <errno.h>
#include <config.h>
#include <asm/arch/typedefs.h>

#define GPIO61_MODE 			0x10211340
#define GPIO61_DIR_MODE 		0x10211010
#define GPIO61_DOUT_MODE 		0x10211110

#define GPIO83_MODE 			0x10211350
#define GPIO83_DIR_MODE 		0x10211020
#define GPIO83_DOUT_MODE 		0x10211120

#define CMD_RET_SUCCESS 1
#define CMD_RET_FAIL 0
#define TRUE 1
#define FALSE 0
#define BLE_FORMAT_HEADER_LEN 4
#define BLE_PW_ON_IDX 0
#define BLE_HCI_RESET_IDX 1
#define BLE_HCI_SET_ADV_PARAM_IDX 2
#define BLE_HCI_SET_ADV_DATA_IDX 3
#define BLE_HCI_SET_SCAN_RESP_DATA_IDX 4
#define BLE_HCI_SET_ADV_ENABLE_IDX 5
#define GPIO_CLEAR 0
#define GPIO_SET 1
// BLE EXPECTED RX Messgae
const char  pw_on_correct_code[] = {0x04, 0xe4, 0x05, 0x02, 0x06, 0x01, 0x00, 0x00};
const char  hic_reset_correct_code[] = {0x04, 0x0E, 0x04, 0x01, 0x03, 0x0C, 0x00};
const char  hic_le_set_adv_param_correct_code[] = {0x04, 0x0E, 0x04, 0x01, 0x06, 0x20, 0x00};
const char  hic_le_set_adv_data_correct_code[] = {0x04, 0x0E, 0x04, 0x01, 0x08, 0x20, 0x00};
const char  hic_le_set_scan_resp_data_correct_code[] = {0x04, 0x0E, 0x04, 0x01, 0x09, 0x20, 0x00};
const char  hic_le_set_adv_en_correct_code[] = {0x04, 0x0E, 0x04, 0x01, 0x0A, 0x20, 0x00};

//BLE TX Command
const char bt_power_on_tx_cmd[] = {0x01, 0x6f, 0xfc, 0x06, 0x01, 0x06, 0x02, 0x00, 0x00, 0x01};

const char hci_reset_tx_cmd[] = {0x01, 0x03, 0x0C, 0x00};

const char hci_le_set_adv_param_tx_cmd[] =
{
	0x01, 0x06, 0x20, 0x0f,
	0xa0, 0x00, //Advertising Interval Min (x0.625ms)
	0xa0, 0x00, //Advertising Interval Max (x0.625ms)
	0x02, //Advertising Type
	0x00, //Own Address Type
	0x00, //Peer Address Type
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Peer Address
	0x07, //Advertising Channel Map
	0x00 //Advertising Filter Policy
};
//Sample1: MT7915_BLE_ADV_TEST
const char hci_le_set_adv_data_tx_cmd[] =
{
	0x01, 0x08, 0x20, 0x20,
	0x18, //Advertising Data Length, Total 31 bytes
	0x02, 0x01, 0x1A, 0x14, 0x09, 0x4d, // Advertising Data Byte0~Byte6
	0x54, 0x37, 0x39, 0x31, 0x35, 0x5f, // Advertising Data Byte7~Byte12
	0x42, 0x4c, 0x45, 0x5f, 0x41, 0x44, // Advertising Data Byte13~Byte18
	0x56, 0x5f, 0x54, 0x45, 0x53, 0x54, // Advertising Data Byte19~Byte24
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Advertising Data Byte25~Byte30
	0x00 // Advertising Data Byte31
};

const char hci_le_set_scan_resp_data_tx_cmd[] =
{
	0x01, 0x09, 0x20, 0x20,
	0x18, //Scan Response Data Length, Total 31 bytes
	0x02, 0x01, 0x1A, 0x14, 0x09, 0x4d, // Scan Response Data Byte0~Byte6
	0x54, 0x37, 0x39, 0x31, 0x35, 0x5f, // Scan Response Data Byte7~Byte12
	0x42, 0x4c, 0x45, 0x5f, 0x41, 0x44, // Scan Response Data Byte13~Byte18
	0x56, 0x5f, 0x54, 0x45, 0x53, 0x54, // Scan Response Data Byte19~Byte24
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Scan Response Data Byte25~Byte30
	0x00 // Scan Response Data Byte31
};

const char hci_le_set_adv_enable_tx_cmd[] = {0x01, 0x0A, 0x20, 0x01, 0x01}; //Advertising Enable/Disable

extern volatile unsigned int g_uart;
extern volatile unsigned int g_2nd_uart;
extern struct serial_device *default_serial_console(void);
extern struct serial_device *mtk_2nd_serial_console(void);

static int get_ble_tx_cmd(u8 idx, u8 tx_cmd_buf[])
{
	u8 len = 0;

	memset(tx_cmd_buf, 0x0, sizeof(tx_cmd_buf));
	switch(idx)
	{
		case BLE_PW_ON_IDX:
			len = sizeof(bt_power_on_tx_cmd) / sizeof(u8);
			memcpy(tx_cmd_buf, bt_power_on_tx_cmd, sizeof(bt_power_on_tx_cmd));
			break;
		case BLE_HCI_RESET_IDX:
			len = sizeof(hci_reset_tx_cmd) / sizeof(u8);
			memcpy(tx_cmd_buf, hci_reset_tx_cmd, sizeof(hci_reset_tx_cmd));
			break;
		case BLE_HCI_SET_ADV_PARAM_IDX:
			len = sizeof(hci_le_set_adv_param_tx_cmd) / sizeof(u8);
			memcpy(tx_cmd_buf, hci_le_set_adv_param_tx_cmd, sizeof(hci_le_set_adv_param_tx_cmd));
			break;
		case BLE_HCI_SET_ADV_DATA_IDX:
			len = sizeof(hci_le_set_adv_data_tx_cmd) / sizeof(u8);
			memcpy(tx_cmd_buf, hci_le_set_adv_data_tx_cmd, sizeof(hci_le_set_adv_data_tx_cmd));
			break;
		case BLE_HCI_SET_SCAN_RESP_DATA_IDX:
			len = sizeof(hci_le_set_scan_resp_data_tx_cmd) / sizeof(u8);
			memcpy(tx_cmd_buf, hci_le_set_scan_resp_data_tx_cmd, sizeof(hci_le_set_scan_resp_data_tx_cmd));
			break;
		case BLE_HCI_SET_ADV_ENABLE_IDX:
			len = sizeof(hci_le_set_adv_enable_tx_cmd) / sizeof(u8);
			memcpy(tx_cmd_buf, hci_le_set_adv_enable_tx_cmd, sizeof(hci_le_set_adv_enable_tx_cmd));
			break;
		default:
			break;
	}
	return len;
}
static int judge_rx_msg(u8 msg_idx, u8 rx_msg_buf[])
{
	u8 ret = 0;
	u8 num = 0;

	switch(msg_idx)
	{
		case BLE_PW_ON_IDX:
			num = sizeof(pw_on_correct_code);
			ret = memcmp( rx_msg_buf, pw_on_correct_code, num);
			//printf("[judge_rx_msg]: num=%d, ret=%d\n", num, ret);
			break;
		case BLE_HCI_RESET_IDX:
			num = sizeof(hic_reset_correct_code);
			ret = memcmp( rx_msg_buf, hic_reset_correct_code, num);
			//printf("[judge_rx_msg]: num=%d, ret=%d\n", num, ret);
			break;
		case BLE_HCI_SET_ADV_PARAM_IDX:
			num = sizeof(hic_le_set_adv_param_correct_code);
			ret = memcmp( rx_msg_buf, hic_le_set_adv_param_correct_code, num);
			//printf("[judge_rx_msg]: num=%d, ret=%d\n", num, ret);
			break;
		case BLE_HCI_SET_ADV_DATA_IDX:
			num = sizeof(hic_le_set_adv_data_correct_code);
			ret = memcmp( rx_msg_buf, hic_le_set_adv_data_correct_code, num);
			//printf("[judge_rx_msg]: num=%d, ret=%d\n", num, ret);
			break;
		case BLE_HCI_SET_SCAN_RESP_DATA_IDX:
			num = sizeof(hic_le_set_scan_resp_data_correct_code);
			ret = memcmp( rx_msg_buf, hic_le_set_scan_resp_data_correct_code, num);
			//printf("[judge_rx_msg]: num=%d, ret=%d\n", num, ret);
			break;
		case BLE_HCI_SET_ADV_ENABLE_IDX:
			num = sizeof(hic_le_set_adv_en_correct_code);
			ret = memcmp( rx_msg_buf, hic_le_set_adv_en_correct_code, num);
			//printf("[judge_rx_msg]: num=%d, ret=%d\n", num, ret);
			break;
		default:
			break;
	}
	if(ret == 0 )
		return CMD_RET_SUCCESS;
	else
		return CMD_RET_FAIL;
}

static int do_mtk_ble_adv(cmd_tbl_t *cmdtp, int flag, int argc,	char *const argv[])
{
	// START BT PROCESS
	//struct udevice *uart;
	u8 bt_tx_cmd[40]={0x0};
	u8 bt_rx_msg[40]={0x0};
	u8 num = 0;
	u8 len = 0;
	u8 i = 0;
	u8 ch = 0;
	u8 curr_step_pass = 0;
	int ret;
	u32 start_timep;
	u32 curr_timep;
	u32 value = 0;

	printf("=========================GPIO INIT=====================\n");
	printf("GPIO61 initial to push GPIO28 on MT7915\n");
	// init gpio61 to gpio mode
	value = DRV_Reg32(GPIO61_MODE);
	value = value | (0x1 << 12);
	DRV_WriteReg32(GPIO61_MODE, value);
	value = DRV_Reg32(GPIO61_MODE);
	printf("GPIO61_MODE value: %x\n", value);

	// init gpio61 to output mode
	value = DRV_Reg32(GPIO61_DIR_MODE);
	value = value | (0x1 << 29);
	DRV_WriteReg32(GPIO61_DIR_MODE, value);
	value = DRV_Reg32(GPIO61_DIR_MODE);
	printf("GPIO61_DIR_MODE value: %x\n", value);

	// init gpio61 data to high
	value = DRV_Reg32(GPIO61_DOUT_MODE);
	value = value | (0x1 << 29);
	DRV_WriteReg32(GPIO61_DOUT_MODE, value);
	value = DRV_Reg32(GPIO61_DOUT_MODE);
	printf("GPIO61_DOUT_MODE value: %x\n", value);

	printf("GPIO83(PERST0) initial to signal trigger PCIE on MT7915\n");
	// init gpio83(PERST0) to gpio mode
	value = DRV_Reg32(GPIO83_MODE);
	value = value | (0x1 << 28);
	DRV_WriteReg32(GPIO83_MODE, value);
	value = DRV_Reg32(GPIO83_MODE);
	printf("GPIO83_MODE value: %x\n", value);

	// init gpio83(PERST0)  to output mode
	value = DRV_Reg32(GPIO83_DIR_MODE);
	value = value | (0x1 << 19);
	DRV_WriteReg32(GPIO83_DIR_MODE, value);
	value = DRV_Reg32(GPIO83_DIR_MODE);
	printf("GPIO83_DIR_MODE value: %x\n", value);

	// trigger gpio83(PERST0) outpur data to high
	value = DRV_Reg32(GPIO83_DOUT_MODE);
	value = value | (0x1 << 19);
	DRV_WriteReg32(GPIO83_DOUT_MODE, value);
	value = DRV_Reg32(GPIO83_DOUT_MODE);
	printf("GPIO83_DOUT_MODE value: %x\n", value);
	mdelay(100);

	printf("=========================UART_3 INIT=====================\n");
	mtk_2nd_serial_console()->start();
	mtk_2nd_serial_console()->setbrg();
	//FLOW 1: BT Power On
	printf("=========================FLOW 1=====================\n");
	memset(bt_tx_cmd, 0x0, sizeof(bt_tx_cmd));
	len = sizeof(pw_on_correct_code)/sizeof(u8);
	ret = get_ble_tx_cmd(BLE_PW_ON_IDX, bt_tx_cmd);
	if(ret > 0)
	{
		num = ret;
		printf("[BT Power On]: Tx Cmd=");
		for (i = 0; i < num; i++)
		{
			mtk_2nd_serial_console()->putc(bt_tx_cmd[i]);
			printf("%02x ",bt_tx_cmd[i]);
		}
		printf("\n");
	}
	printf("len= %d, num= %d\n", len, num);

	i = 0;
	memset(bt_rx_msg, 0x0, sizeof(bt_rx_msg));
	printf("[BT Power On]: Rx Msg=");
	start_timep = get_timer(0);
	while (i < len) {
		if (mtk_2nd_serial_console()->tstc()){
			ch = mtk_2nd_serial_console()->getc();
			memcpy(&bt_rx_msg[i], &ch, sizeof(ch));
			printf("%02x ",bt_rx_msg[i]);
			i++;
		}

		curr_timep = get_timer(0);
		if(curr_timep - start_timep > 1500)
			break;
	}
	printf("\n");
	ret = judge_rx_msg(BLE_PW_ON_IDX, bt_rx_msg);
	if(ret == CMD_RET_SUCCESS){
		curr_step_pass = TRUE;
		printf("[BT Power On Result] Success\n");
	}else
		printf("[BT Power On Result] Fail\n");

	//FLOW 2: HCI_RESET
	printf("\n=========================FLOW 2=====================\n");
	if(curr_step_pass == TRUE)
	{
		memset(bt_tx_cmd, 0x0, sizeof(bt_tx_cmd));
		len = sizeof(hic_reset_correct_code)/sizeof(u8);
		ret = get_ble_tx_cmd(BLE_HCI_RESET_IDX, bt_tx_cmd);
		if(ret > 0)
		{
			num = ret;
			printf("[HCI RESET]: Tx Cmd=");
			for (i = 0; i < num; i++)
			{
				mtk_2nd_serial_console()->putc(bt_tx_cmd[i]);
				printf("%02x ",bt_tx_cmd[i]);
			}
			printf("\n");
		}
		printf("len= %d, num= %d\n", len, num);

		i = 0;
		memset(bt_rx_msg, 0x0, sizeof(bt_rx_msg));
		printf("[HCI RESET]: Rx Msg=");
		start_timep = get_timer(0);
		while (i < len) {
			if (mtk_2nd_serial_console()->tstc()){
				ch = mtk_2nd_serial_console()->getc();
				memcpy(&bt_rx_msg[i], &ch, sizeof(ch));
				printf("%02x ",bt_rx_msg[i]);
				i++;
			}

			curr_timep = get_timer(0);
			if(curr_timep - start_timep > 1500)
				break;
		}
		printf("\n");

		ret = judge_rx_msg(BLE_HCI_RESET_IDX, bt_rx_msg);
		if(ret == CMD_RET_SUCCESS){
			curr_step_pass = TRUE;
			printf("[HIC RESET Result] Success\n");
		}
		else{
			curr_step_pass = FALSE;
			printf("[HIC RESET Result] Fail\n");
		}
	}

	//FLOW 3: HCI_LE_Set_Advertising_Parameters
	printf("\n=========================FLOW 3=====================\n");
	if(curr_step_pass == TRUE)
	{	memset(bt_tx_cmd, 0x0, sizeof(bt_tx_cmd));
		len = sizeof(hic_le_set_adv_param_correct_code)/sizeof(u8);
		ret = get_ble_tx_cmd(BLE_HCI_SET_ADV_PARAM_IDX, bt_tx_cmd);
		if(ret > 0)
		{
			num = ret;
			printf("[HCI LE_Set Advertising Parameters]: Tx Cmd=");
			for (i = 0; i < num; i++)
			{
				mtk_2nd_serial_console()->putc(bt_tx_cmd[i]);
				printf("%02x ",bt_tx_cmd[i]);
			}
			printf("\n");
		}
		printf("len= %d, num= %d\n", len, num);

		i = 0;
		memset(bt_rx_msg, 0x0, sizeof(bt_rx_msg));
		printf("[HCI LE_Set Advertising Parameters]: Rx Msg=");
		start_timep = get_timer(0);
		while (i < len) {
			if (mtk_2nd_serial_console()->tstc()){
				ch = mtk_2nd_serial_console()->getc();
				memcpy(&bt_rx_msg[i], &ch, sizeof(ch));
				printf("%02x ",bt_rx_msg[i]);
				i++;
			}

			curr_timep = get_timer(0);
			if(curr_timep - start_timep > 1500)
				break;
		}
		printf("\n");

		ret = judge_rx_msg(BLE_HCI_SET_ADV_PARAM_IDX, bt_rx_msg);
		if(ret == CMD_RET_SUCCESS){
			curr_step_pass = TRUE;
			printf("[HIC LE  SET ADVERTISING PARAMETER Result] Success\n");
		}
		else{
			curr_step_pass = FALSE;
			printf("[HIC LE  SET ADVERTISING PARAMETER Result]  Fail\n");
		}
	}

	//FLOW 4: HCI_LE_Set_Advertising_Data
	printf("\n=========================FLOW 4=====================\n");
	if(curr_step_pass == TRUE)
	{
		memset(bt_tx_cmd, 0x0, sizeof(bt_tx_cmd));
		len = sizeof(hic_le_set_adv_data_correct_code)/sizeof(u8);
		ret = get_ble_tx_cmd(BLE_HCI_SET_ADV_DATA_IDX, bt_tx_cmd);
		if(ret > 0)
		{
			num = ret;
			printf("[HCI LE_Set Advertising Data]: Tx Cmd=");
			for (i = 0; i < num; i++)
			{
				mtk_2nd_serial_console()->putc(bt_tx_cmd[i]);
				printf("%02x ",bt_tx_cmd[i]);
			}
			printf("\n");
		}
		printf("len= %d, num= %d\n", len, num);

		i = 0;
		memset(bt_rx_msg, 0x0, sizeof(bt_rx_msg));
		printf("[HCI LE_Set Advertising Data]: Rx Msg=");
		start_timep = get_timer(0);
		while (i < len) {
			if (mtk_2nd_serial_console()->tstc()){
				ch = mtk_2nd_serial_console()->getc();
				memcpy(&bt_rx_msg[i], &ch, sizeof(ch));
				printf("%02x ",bt_rx_msg[i]);
				i++;
			}

			curr_timep = get_timer(0);
			if(curr_timep - start_timep > 1500)
				break;
		}
		printf("\n");

		ret = judge_rx_msg(BLE_HCI_SET_ADV_DATA_IDX, bt_rx_msg);
		if(ret == CMD_RET_SUCCESS){
			curr_step_pass = TRUE;
			printf("[HIC LE  SET ADVERTISING DATA Result] Success\n");
		}
		else{
			curr_step_pass = FALSE;
			printf("[HIC LE  SET ADVERTISING DATA Result] Fail\n");
		}
	}

	//FLOW 5: HCI_LE_Set_Scan_Response_Data
	printf("\n=========================FLOW 5=====================\n");
	if(curr_step_pass == TRUE)
	{
		memset(bt_tx_cmd, 0x0, sizeof(bt_tx_cmd));
		len = sizeof(hic_le_set_scan_resp_data_correct_code)/sizeof(u8);
		ret = get_ble_tx_cmd(BLE_HCI_SET_SCAN_RESP_DATA_IDX, bt_tx_cmd);
		if(ret > 0)
		{
			num = ret;
			printf("[HCI LE_Set Scan Response Data]: Tx Cmd=");
			for (i = 0; i < num; i++)
			{
				mtk_2nd_serial_console()->putc(bt_tx_cmd[i]);
				printf("%02x ",bt_tx_cmd[i]);
			}
			printf("\n");
		}
		printf("len= %d, num= %d\n", len, num);

		i = 0;
		memset(bt_rx_msg, 0x0, sizeof(bt_rx_msg));
		printf("[HCI LE_Set Scan Response Data ]: Rx Msg=");
		start_timep = get_timer(0);
		while (i < len) {
			if (mtk_2nd_serial_console()->tstc()){
				ch = mtk_2nd_serial_console()->getc();
				memcpy(&bt_rx_msg[i], &ch, sizeof(ch));
				printf("%02x ",bt_rx_msg[i]);
				i++;
			}

			curr_timep = get_timer(0);
			if(curr_timep - start_timep > 1500)
				break;
		}
		printf("\n");

		ret = judge_rx_msg(BLE_HCI_SET_SCAN_RESP_DATA_IDX, bt_rx_msg);
		if(ret == CMD_RET_SUCCESS){
			curr_step_pass = TRUE;
			printf("[HIC LE  SET SCAN RESPONSE Result] Success\n");
		}
		else{
			curr_step_pass = FALSE;
			printf("[HIC LE  SET SCAN RESPONSE Result] Fail\n");
		}

	}
	//FLOW 6: HCI_LE_Set_Advertising_Enable
	printf("\n=========================FLOW 6=====================\n");
	if(curr_step_pass == TRUE)
	{
		memset(bt_tx_cmd, 0x0, sizeof(bt_tx_cmd));
		len = sizeof(hic_le_set_adv_en_correct_code)/sizeof(u8);
		ret = get_ble_tx_cmd(BLE_HCI_SET_ADV_ENABLE_IDX, bt_tx_cmd);
		if(ret > 0)
		{
			num = ret;
			printf("[HCI LE_Set Advertising Enable]: Tx Cmd=");
			for (i = 0; i < num; i++)
			{
				mtk_2nd_serial_console()->putc(bt_tx_cmd[i]);
				printf("%02x ",bt_tx_cmd[i]);
			}
			printf("\n");
		}
		printf("len= %d, num= %d\n", len, num);
		i = 0;

		memset(bt_rx_msg, 0x0, sizeof(bt_rx_msg));
		printf("[HCI LE_Set Advertising Enable]: Rx Msg=");
		start_timep = get_timer(0);
		while (i < len) {
			if (mtk_2nd_serial_console()->tstc()) {
				ch = mtk_2nd_serial_console()->getc();
				memcpy(&bt_rx_msg[i], &ch, sizeof(ch));
				printf("%02x ",bt_rx_msg[i]);
				i++;
			}
			curr_timep = get_timer(0);
			if(curr_timep - start_timep > 1500)
				break;
		}
		printf("\n");

		ret = judge_rx_msg(BLE_HCI_SET_ADV_ENABLE_IDX, bt_rx_msg);
		if(ret == CMD_RET_SUCCESS){
			curr_step_pass = TRUE;
			printf("[HIC LE  SET ADVERISTING ENABLE Result] Success\n");
		}
		else{
			curr_step_pass = FALSE;
			printf("[HIC LE  SET ADVERISTING ENABLE Result] Fail\n");
		}
	}

	return CMD_RET_SUCCESS;
}
U_BOOT_CMD(mtk_ble_adv, 1, 0, do_mtk_ble_adv,
			"Test BLE Adv",
			"this feature is based on uart3 function enable\n"
);
