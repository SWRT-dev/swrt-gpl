#include "precomp.h"

struct mtd_info_user {
	u_char type;
	u_int32_t flags;
	u_int32_t size;
	u_int32_t erasesize;
	u_int32_t oobblock;
	u_int32_t oobsize;
	u_int32_t ecctype;
	u_int32_t eccsize;
};

struct erase_info_user {
	u_int32_t start;
	u_int32_t length;
};

#define MEMGETINFO	_IOR('M', 1, struct mtd_info_user)
#define MEMERASE	_IOW('M', 2, struct erase_info_user)
#define MEMGETBADBLOCK		_IOW('M', 11, __kernel_loff_t)
#if defined CONFIG_NVRAM_RW_FILE
#define NVRAM_PATH "/etc/nvram"
#endif

int mtd_open(const char *name, int flags)
{
	FILE *fp;
	char dev[80];
	int i, ret;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, name)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				if ((ret = open(dev, flags)) < 0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
					ret = open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}
	return -1;
}

int flash_read(char *buf, off_t from, size_t len)
{
	int fd, ret;
#if defined CONFIG_NVRAM_RW_FILE
	FILE *fp = fopen(NVRAM_PATH, "r");
	
	if (fp == NULL) {
		perror(__FUNCTION__);
		return -1;
	}
	rewind(fp);
	fseek(fp, from, SEEK_SET);
	ret = (int) fread(buf, 1, len, fp);
	fclose(fp);
#else
	struct mtd_info_user info;

	fd = mtd_open("Factory", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes - %zd > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	lseek(fd, from, SEEK_SET);
	ret = read(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Reading from mtd failed\n");
		close(fd);
		return -1;
	}

	close(fd);
#endif
	return ret;
}

#define min(x,y) ({ typeof(x) _x = (x); typeof(y) _y = (y); (void) (&_x == &_y); _x < _y ? _x : _y; })


/*
  workaround due to openwrt 32 bit toolchain, off_t size is still 8 byes, then len parameter will be zero.
  tmp use the unsigned int instead of off_t or unsigned long long
*/
int flash_write(char *buf, unsigned int to, size_t len)
{
	int fd, ret = 0;
#if defined CONFIG_NVRAM_RW_FILE
	FILE *fp;
	if ((fp = fopen(NVRAM_PATH, "r+")) == NULL &&
	    (fp = fopen(NVRAM_PATH, "w+")) == NULL) {
		perror(__FUNCTION__);
		return -1;
	}
	rewind(fp);
	fseek(fp, to, SEEK_SET);
	ret = (int) fwrite(buf, 1, len, fp);
	fclose(fp);
#else
	char *bak = NULL;
	struct mtd_info_user info;
	struct erase_info_user ei;
	size_t rest = 0;
	unsigned long long offset = (unsigned long long)to;

	fd = mtd_open("Factory", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes: %zd > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	while (len > 0) {
		if ((int)info.oobsize != 0) {

			int ret = ioctl(fd, MEMGETBADBLOCK, &offset);

			if (ret < 0) {
				fprintf(stderr, "Check Bad failed\n");
			}
			else if (ret) {
				fprintf(stderr, "skip bad block %x\n   ", to);
				if (lseek(fd, info.erasesize, SEEK_CUR) == -1) {
					fprintf(stderr, "lseek failed.\n");
					close(fd);
					return -1;
				}
				to += info.erasesize;
				offset += info.erasesize;
				printf("Bad block detected\n");
				continue;
			}

		}

		if ((to & (info.erasesize-1)) || (len < info.erasesize)) {
			int piece_size;
			unsigned int piece, bakaddr;

			bak = (char *)malloc(info.erasesize);
			if (bak == NULL) {
				fprintf(stderr, "Not enough memory\n");
				close(fd);
				return -1;
			}

			bakaddr = to & ~(info.erasesize - 1);
			lseek(fd, bakaddr, SEEK_SET);

			ret = read(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Reading from mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			piece = to & (info.erasesize - 1);
			rest = info.erasesize - piece;
			piece_size = min(len, rest);
			memcpy(bak + piece, buf, piece_size);

			ei.start = bakaddr;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			lseek(fd, bakaddr, SEEK_SET);
			ret = write(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			free(bak);
			buf += piece_size;
			to += piece_size;
			len -= piece_size;
		}
		else {
			ei.start = to;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				return -1;
			}

			ret = write(fd, buf, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			buf += info.erasesize;
			to += info.erasesize;
			len -= info.erasesize;
		}
	}

	close(fd);
#endif
	return ret;
}

void memcpy_exs(unsigned char *dst, unsigned char *src, UINT16 len)
{
	UINT32 i;
	UINT16 *pDst, *pSrc;

	pDst = (UINT16 *) dst;
	pSrc = (UINT16 *) src;

	for (i = 0; i < (len >> 1); i++) {
		*pDst = be2cpu16(*pSrc);
		pDst++;
		pSrc++;
	}

	if ((len % 2) != 0) {
		os_memcpy(pDst, pSrc, (len % 2));
		*pDst = be2cpu16(*pDst);
	}
}

int mt_e2p_read(UINT8 *inpkt)
{
	INT32 ret = 0;
	struct racfghdr *p_racfgh = NULL;
	UINT16 length;
	unsigned char *data = NULL;

	p_racfgh = (struct racfghdr *)inpkt;
	length = be2cpu16(p_racfgh->length);
	data = p_racfgh->data;

	ate_printf(MSG_INFO, "%s: HQA_ReadBulkEEPROM\n", __func__);

	if ((length - 2) == 16) {
		memcpy_exs(e2p_buffer + e2p_offset, data + 2, (length - 2));

		e2p_offset += 16;
		if (e2p_offset == EEPROM_SIZE)
			e2p_offset = 0;
	}

	return ret;
}

int mt_e2p_write(UINT8 *inpkt, UINT32 offset_int)
{
	INT32 ret = 0;
	struct racfghdr *p_racfgh = NULL;
	UINT32 e2p_mode;
	UINT16 command_id, offset, length;
	unsigned char *data = NULL;

	p_racfgh = (struct racfghdr *)inpkt;
	command_id = be2cpu16(p_racfgh->comand_id);
	data = p_racfgh->data;

	if (command_id == HQA_WriteBulkEEPROM) {
		os_memcpy(&offset, data, sizeof(offset));
		data += sizeof(offset);
		os_memcpy(&length, data, sizeof(length));
		data += sizeof(length);

		offset = be2cpu16(offset);
		length = be2cpu16(length);
		memcpy_exs(e2p_buffer + offset, data, length);

		ate_printf(MSG_INFO, "%s: HQA_WriteBulkEEPROM\n", __func__);

		if ((offset + length) <= EEPROM_SIZE)
			flash_write((char *)e2p_buffer+offset, (offset + offset_int), length);
		else
			ate_printf(MSG_ERROR, "exceed EEPROM size(%d), offset=%u, length=%u\n", EEPROM_SIZE, offset, length);
	}

	if (command_id == HQA_WriteBufferDone) {
		os_memcpy(&e2p_mode, data, sizeof(e2p_mode));
		e2p_mode = be2cpu32(e2p_mode);

		ate_printf(MSG_INFO, "%s: HQA_WriteBufferDone\n", __func__);

		if (e2p_mode == E2P_FLASH_MODE)
			flash_write((char *)e2p_buffer, offset_int, EEPROM_SIZE);
	}

	return ret;
}
