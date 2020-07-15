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

#ifndef _FTC_CORE_RESPONSE_H_
#define _FTC_CORE_RESPONSE_H_
#include "macro.h"
#include <json-glib/json-glib.h>
#include <string>
#include <unordered_map>



BEGIN_FTC_CORE

class Response{
public:
    Response(); 
    Response(const std::string &url); 
    Response(const Response &resp); 
    virtual ~Response(); 
    void setJsonNode(JsonNode* node); 
    JsonNode *getJsonNode() const; 
    void setUrl(const char *url); 
    const std::string &getUrl(); 
    void setResponse(const std::string &respMessage); 
    void setResponse(const char *respMessage); 
    void setResponse(const char *respMessage, const int messageLen); 
    void setHeaderMap(const char *headerName, const char *headerValue);
    void setExpiredSession(bool isSessionExpired);
    const std::string &getResponseMessage(); 
    const std::string getResponseHeaderValue(const char *headerName);
    bool isSuccess() ; 
    bool isExpiredSession();
    FTC_SYNTHESIZE(int , _status, Status)
private: 
    JsonNode* _node ;
    std::string _url; 
    std::string _respMessage ; 
    bool        _isSessionExpired ;
    std::unordered_map<std::string, std::string> _respHeader ;
};

typedef struct _AsyncResult {
    Response *resp;
    gpointer user_data;
} AsyncResult;

END_FTC_CORE
#endif /* _FTC_CORE_RESPONSE_H_ */