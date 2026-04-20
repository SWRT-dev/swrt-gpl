#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h> 
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/file.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/sha.h> 
#include <openssl/bio.h>

#include <shared.h>
#include <libasc.h>
#include <shutils.h>

#include<json.h>
#include "networkmap.h"
#include "sm.h"
#include "download.h"
#include "protect_name.h"
#include "shared_func.h"
#include "JSON_checker.h"
#define ASD_SHA256_LEN 32
#define NMP_SIG_LEN 256
#ifndef LIVE_UPDATE_RSA_VERSION
#define LIVE_UPDATE_RSA_VERSION ""
#endif

size_t encrypt_string(char *in, char *out, size_t maxlen)
{
	if(!in || !out || maxlen < pw_enc_blen(in))
		return NMP_FAIL;
	memset(out, 0, maxlen);
	pw_enc(in, out, 0);
	return strlen(out);
}

static char *_convert_ascii_to_hex(const char *ascii_str, const size_t ascii_len, char *hex_str, const size_t hex_len)
{
	int i;

	if(!ascii_str || !hex_str || hex_len <= (ascii_len * 2))	//hex_str need a end-string character in its array.
		return NULL;
	
	for(i = 0; i < ascii_len; ++i)
		sprintf(hex_str + (i * 2), "%02X", ascii_str[i]);
	return hex_str;
}

int encrypt_file(const char *src_file, const char *dst_file, const int with_sig)
{
	size_t src_sz, dst_sz, hex_len;
	unsigned char sig_buf[NMP_SIG_LEN];
	char *src_buf = NULL, *dst_buf = NULL, *hex_str = NULL;
	FILE *fp;
	int ret = NMP_FAIL;
	
	if(!src_file || !dst_file)
		return ret;
	
	//read file content
	src_sz = f_size(src_file);
	printf("[%s] aa src_sz = %d,  NMP_SIG_LEN = %d \n", __func__, src_sz, NMP_SIG_LEN);
	if(!src_sz || (with_sig && src_sz <= NMP_SIG_LEN))
	{
		NMP_DEBUG("[%s] File size is invalid (%s)!\n", __func__, src_file);
		return ret;
	}
	
	if(with_sig)
		src_sz -= NMP_SIG_LEN;
	printf("[%s] bb src_sz = %d,  NMP_SIG_LEN = %d \n", __func__, src_sz, NMP_SIG_LEN);
	fp = fopen(src_file, "r");		
	if(fp)
	{
		src_buf = calloc(src_sz + 1, 1);
		if(!src_buf)
		{
			NMP_DEBUG("[%s] Memory alloc fail!\n", __func__);
			fclose(fp);
			return ret;
		}
		fread(src_buf, 1, src_sz, fp);
		if(with_sig){
			memset(sig_buf, 0, sizeof(sig_buf));
			fread(sig_buf, 1, NMP_SIG_LEN, fp);
		}
		fclose(fp);
	}
	else
	{
		NMP_DEBUG("[%s] Cannot open file (%s)!\n", __func__, src_file);
		return ret;
	}	

	//convert file content to hex string
	hex_len = src_sz * 2 + 1;
	hex_str = calloc(hex_len, 1);
	if(!hex_str)
	{
		NMP_DEBUG("[%s] Memory alloc fail!\n", __func__);
		SAFE_FREE(src_buf);
		return ret;
	}
	
	if(!_convert_ascii_to_hex(src_buf, src_sz, hex_str, hex_len))
	{
		NMP_DEBUG("[%s] _convert_ascii_to_hex fail!\n", __func__);
		SAFE_FREE(hex_str);
		SAFE_FREE(src_buf);
		return ret;
	}

	//the original file content would not be used, free it.
	SAFE_FREE(src_buf);

	//encrypt the hex string
	dst_sz = pw_enc_blen(hex_str);

	dst_buf = calloc(dst_sz + 1, 1);
	if(!dst_buf)
	{
		NMP_DEBUG("[%s] Memory alloc fail!\n", __func__);
		SAFE_FREE(hex_str);
		return ret;
	}
	pw_enc(hex_str, dst_buf, 0);
	if(dst_buf[0] != '\0')
	{
		fp = fopen(dst_file, "w");
		if(fp)
		{
			fwrite(dst_buf, 1, strlen(dst_buf), fp);
			if(with_sig)
				fwrite(sig_buf, 1, NMP_SIG_LEN, fp);
				
			fclose(fp);
			ret = NMP_SUCCESS;
		}
		else
		{
			NMP_DEBUG("[%s] Cannot open file (%s)!\n", __func__, dst_file);
		}
	}
	else
	{
		NMP_DEBUG("[%s] Cannot encrypt content.\n", __func__);
	}
	
	SAFE_FREE(hex_str);
	SAFE_FREE(dst_buf);
	return ret;
}

int download_file(const char* file_name, const char *local_file_path)
{
	char dl_path[64], ver[4];
	if(!file_name || !local_file_path || !internet_ready())
		return NMP_FAIL;

	if(!strcmp(file_name, version_name[0])){
		strlcpy(ver, LIVE_UPDATE_RSA_VERSION, sizeof(ver));
		if(ver[0] == 0)
			strlcpy(ver, "0", sizeof(ver));
		snprintf(dl_path, sizeof(dl_path), "%s.%s%s", "nmpv2", ver, dl_path_file_name);
	}else
		snprintf(dl_path, sizeof(dl_path), "%s%s", "nmpv2", dl_path_file_name);
	printf("[%s] file_name = %s \n", __func__, file_name);
	printf("[%s] dl_path = %s \n", __func__, dl_path);
	printf("[%s] local_file_path = %s \n", __func__, local_file_path);
	nvram_set("nmp_type", file_name);
	return (curl_download_file(NETWORKMAP_DB, dl_path, local_file_path) == LIBASC_SUCCESS)? NMP_SUCCESS: NMP_FAIL;
}

static int _verify_hex_str(const char *str, const size_t len)
{
    int i;
    
    if(str)
    {
        for(i = 0; i < len; ++i)
        {
            if((str[i] < '0' || str[i] > '9') &&  //check number
                (str[i] <'A' || str[i] > 'F') &&    //check A~F
                (str[i] < 'a' || str[i] > 'f')) //check a~f
                return NMP_FAIL;
        }
        return NMP_SUCCESS;
    }
    return NMP_FAIL;
}

static char *_convert_hex_to_ascii(const char *hex_str, const size_t hex_len, char *ascii_str, const size_t ascii_len)
{
	int i, j;
	char hex[5] = {'0', 'x', '0', '0', '\0'}, *end;

	if(!ascii_str || !hex_str || ascii_len <= (hex_len / 2))	//ascii_str need a end-string character in its array.
		return NULL;
	
	for(i = 0, j = 0; i < hex_len; i += 2, ++j)
	{
		hex[2] = hex_str[i];
		hex[3] = hex_str[i + 1];
		ascii_str[j] = strtol(hex, &end, 16);
	}
	return ascii_str;
}

RSA* _read_public_key()
{
	FILE *fp;
	RSA *pubRSA = NULL;
	fp = fopen(public_key_path[0], "r");
	if(fp)
	{
		if(!PEM_read_RSA_PUBKEY(fp, &pubRSA, NULL, NULL))
		{
			NMP_DEBUG("[%s]PEM_read_RSA_PUBKEY error\n", __func__);
		}
		fclose(fp);
	}
	else
		NMP_DEBUG("[%s]Cannot open public key (%s)\n", __func__, public_key_path[0]);

	return pubRSA;
}	

static int _verify_with_public_key(const char *buf, const size_t buf_len, const unsigned char *signature, const size_t sig_len)
{
	unsigned char md[ASD_SHA256_LEN + 1];
	RSA *pubRSA = NULL;
	int verified = 0;

	if(!buf || !signature)
		return NMP_FAIL;

	pubRSA = _read_public_key();
	if(!pubRSA)
	{
		NMP_DEBUG("[%s]_read_public_key fail!\n", __func__);
		return NMP_FAIL;
	}
	
	memset(md, 0, sizeof(md));
	SHA256(buf, buf_len, md);			
	
	verified = RSA_verify(NID_sha256, md, ASD_SHA256_LEN, signature, sig_len, pubRSA);
	RSA_free(pubRSA);

	return verified == 1 ? NMP_SUCCESS : NMP_FAIL;
}

char *read_file(const char *file, const int check_sig, const int file_enc)
{
	char *buf = NULL, *f_buf = NULL, *hex_str = NULL;
	unsigned char sig_buf[NMP_SIG_LEN + 1] = {0};
	FILE *fp;
	unsigned long sz, dec_sz, buf_len;
	
	memset(sig_buf, 0, sizeof(sig_buf));
	if(!file || access(file, F_OK) == -1)
		return NULL;

	sz = f_size(file);
	if(!sz || (check_sig && sz <= NMP_SIG_LEN))
	{
		NMP_DEBUG("[%s] File size (%d) is invalid (%s)!\n", __func__, sz, file);
		return NULL;
	}

	fp = fopen(file, "r");
	if(fp)
	{
		f_buf = calloc(sz, 1);
		if(!f_buf)
		{
			NMP_DEBUG("[%s] Memory alloc fail!\n", __func__);
			fclose(fp);
			return NULL;
		}
		fread(f_buf, 1, check_sig ? sz - NMP_SIG_LEN : sz, fp);
		if(check_sig){
			fread(sig_buf, 1, NMP_SIG_LEN, fp);
		}
		fclose(fp);
	}
	else
	{
		NMP_DEBUG("[%s] Cannot open file (%s)!\n", __func__, file);
		return NULL;
	}
	if(file_enc)
	{
		dec_sz = pw_dec_len(f_buf);

		if(dec_sz < strlen(f_buf))
			dec_sz = strlen(f_buf);
		hex_str = calloc(dec_sz + 1, 1);
		if(!hex_str)
		{
			NMP_DEBUG("[%s] Memory alloc fail!\n", __func__);
			SAFE_FREE(f_buf);
			return NULL;
		}
		//decrypt content and verify 
		pw_dec(f_buf, hex_str, dec_sz + 1, 0);

		if(_verify_hex_str(hex_str, strlen(hex_str)) == NMP_FAIL)
		{
			NMP_DEBUG("[%s] HEX string is invalid!\n", __func__);
			SAFE_FREE(f_buf);
			SAFE_FREE(hex_str);
			return NULL;
		}
		//convert hex to ascii
		buf_len = strlen(hex_str) / 2 + 1;
		buf = calloc(buf_len, 1);
		if(!buf)
		{
			NMP_DEBUG("[%s] Memory alloc fail!\n", __func__);
			SAFE_FREE(f_buf);
			SAFE_FREE(hex_str);
			return NULL;
		}
		if(!_convert_hex_to_ascii(hex_str, strlen(hex_str), buf, buf_len))
		{
			NMP_DEBUG("[%s] _convert_hex_to_ascii fail!\n", __func__);
			SAFE_FREE(f_buf);
			SAFE_FREE(buf);
			SAFE_FREE(hex_str);
			return NULL;
		}
		SAFE_FREE(hex_str);
	}
	else
	{
		buf = strdup(f_buf);
		if(!buf)
		{
			NMP_DEBUG("[%s] Memory alloc fail!\n", __func__);
			SAFE_FREE(f_buf);
			return NULL;
		}
	}
	SAFE_FREE(f_buf);
	if(buf[0] != '\0')
	{
		if(check_sig)
		{
			if(_verify_with_public_key(buf, strlen(buf), sig_buf, NMP_SIG_LEN) == NMP_SUCCESS)
			{
				return buf;
			}
		}
		else
		{
			return buf;
		}
	}
	SAFE_FREE(buf);
	return NULL;
}

int get_file_sha256_checksum(const char *file, char *checksum, const size_t checksum_len, const int file_enc)
{
    int i = 0;
    unsigned char hash[SHA256_DIGEST_LENGTH];
	char *buf;
	if(!file || !checksum || checksum_len <= 64)
		return NMP_FAIL;
	printf("[%s] file_enc status = %d \n", __func__, file_enc);
	buf = read_file(file, 0, file_enc);
	if(buf){
    	SHA256_CTX sha256;
    	SHA256_Init(&sha256);
    	SHA256_Update(&sha256, buf, strlen(buf));
    	SHA256_Final(hash, &sha256);
		SAFE_FREE(buf);
		memset(checksum, 0, checksum_len);
		for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
			snprintf(checksum + (2 * i), checksum_len - (2 * i), "%02x", hash[i]);
	}
	printf("[%s] file = %s, checksum = %s \n", __func__, file, checksum);
	return NMP_SUCCESS;
}

