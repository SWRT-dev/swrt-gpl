/*base64.h*/  
#ifndef _BASE64_H  
#define _BASE64_H  
  
#include <stdlib.h>  
#include <string.h>  
  
unsigned char *base64_encode(unsigned char *str);  
  
unsigned char *bae64_decode(unsigned char *code);  
  
#endif  
