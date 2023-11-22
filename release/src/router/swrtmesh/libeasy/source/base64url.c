/* gcc -o b64url b64url.c -lssl -lcrypto */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "easy.h"

int LIBEASY_API base64url_encode(uint8_t *out, uint8_t *data, size_t len)
{
	int olen;
	int i;

	olen = EVP_EncodeBlock(out, data, len);
	for (i = 0; i < olen; i++) {
		if (out[i] == '+')
			out[i] = '-';
		else if (out[i] == '/')
			out[i] = '_';
	}

	while (out[olen - 1] == '=') {
		out[olen - 1] = '\0';
		olen--;
	}

	return olen;
}

int LIBEASY_API base64url_decode(uint8_t *out, uint8_t *data, int len)
{
	int pad = 0;
	int olen;
	int i;

	switch (len % 4) {
	case 3:
		pad = 1;
		break;
	case 2:
		pad = 2;
		break;
	case 0:
		break;
	default:
		return -1;
	}

	uint8_t tmp[len + pad];
	uint8_t unb64[len];

	memset(tmp, '=', len + pad);
	memcpy(tmp, data, len);

	for (i = 0; i < len; i++) {
		if (tmp[i] == '-')
			tmp[i] = '+';
		else if (tmp[i] == '_')
			tmp[i] = '/';
	}

	olen = EVP_DecodeBlock(unb64, tmp, len + pad);
	memcpy(out, unb64, olen - pad);
	return olen - pad;
}


#ifdef TEST
int main()
{
	unsigned char data[] = "Key=1234567890";
	size_t datalen = strlen(data);
	uint8_t out[100] = {0};
	int outlen = 0;
	unsigned char data2[100] = {0};
	int data2len = 0;

	outlen = base64url_encode(out, data, datalen);
	printf("base64_enc (len = %d): %s\n", outlen, out);

	data2len = base64url_decode(data2, out, outlen);
	printf("base64_dec (len = %d): %s\n", data2len, data);
}
#endif
