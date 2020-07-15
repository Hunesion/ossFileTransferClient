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

#ifndef _FTC_CORE_HTTP_H_
#define _FTC_CORE_HTTP_H_

#include <unordered_map>
#include <json-glib/json-glib.h>
#include <memory>
#include <string>
#include <map>
#include <libsoup/soup.h>
#include "Response.h"

USING_FTC_CORE ; 



typedef std::multimap<std::string, std::string> HTTP_PARAM ;
typedef std::unordered_map<std::string, std::string> HTTP_HEADER ;
void http_add_param(HTTP_PARAM &param , std::string key, std::string value); 
void http_add_header(HTTP_HEADER &header , std::string key, std::string value); 

std::string http_param_crypto(const HTTP_PARAM *param , bool crypto ); 
std::string http_param_crypto_json(const HTTP_PARAM *param , bool crypto); 

std::shared_ptr<Response> http_send_get(const char *url, const HTTP_PARAM *param = NULL, const HTTP_HEADER *header = NULL); 
std::shared_ptr<Response> http_send_post(const char *url, const HTTP_PARAM *param = NULL, const HTTP_HEADER *header = NULL, bool crypto = false); 
std::shared_ptr<Response> http_send_file_multipart(const char *url , const char *filename , unsigned char **content , long long size , long long offset, long long totalSize,  const HTTP_PARAM *param = NULL , const HTTP_HEADER *header = NULL, bool crypto = false); 
std::shared_ptr<Response> http_send_file_stream(const char *url, const char *filename, unsigned char **content , long long size, const char *type, const char *enckey, const HTTP_PARAM *param = NULL , const HTTP_HEADER *header = NULL, bool crypto = false);
bool http_download_file(const char *url, const HTTP_PARAM *param, const HTTP_HEADER *header, const char *filepath, bool crypto = false);
bool http_send_post_async_task(const char *url , const char *eventName , gpointer eventUserData , HTTP_PARAM *param  ,  const HTTP_HEADER *header,  bool crypto); 
GSList *http_get_cookie_list(); 
void http_clear_cookies() ; 
int http_dispose(); 

#endif 