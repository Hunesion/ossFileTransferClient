/*
* Copyright (c) 2020 Hunesion Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; 
* either version 2.1 of the License, or (at your option) any later version.
* See the file COPYING included with this distribution for more information.
* https://github.com/HuneOpenUp/ossFileTransferClient/blob/master/COPYING
*/

#ifndef _FTC_CORE_BASE64_H_
#define _FTC_CORE_BASE64_H_
unsigned int base64_decode_out(const unsigned char *in, unsigned int in_len, unsigned char **out);
unsigned int base64_decode(const unsigned char *in, unsigned int in_len, unsigned char * out , unsigned int out_len); 
unsigned int get_base64_decode_size(unsigned int in_len);    
unsigned int base64_encode_out(const unsigned char *in, unsigned int in_len, char **out);
unsigned int base64_encode(const unsigned char *in, unsigned int in_len, char * out , unsigned int out_len); 
unsigned int get_base64_encode_size(unsigned int in_len); 

int get_test(); 
#endif /* _FTC_CORE_BASE64_H_ */
