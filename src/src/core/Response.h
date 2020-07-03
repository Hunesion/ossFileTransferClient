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