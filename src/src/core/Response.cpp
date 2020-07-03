#include "Response.h"

BEGIN_FTC_CORE

Response::Response():
_node(NULL), 
_status(0),
_isSessionExpired(false)
{

}

Response::Response(const std::string &url):
_node(NULL), 
_status(0), 
_url(url),
_isSessionExpired(false){

}

Response::Response(const Response &resp){
    this->_status = resp._status ; 
    this->_respMessage = resp._respMessage; 
    this->_node = resp._node ; 
    if (this->_node){
        json_node_ref(this->_node); 
    }
}  

Response::~Response(){
    if (this->_node != NULL){
        json_node_unref(this->_node); 
    }
}  

void Response::setJsonNode(JsonNode* node){
    if (this->_node != NULL){
        json_node_unref(this->_node); 
        this->_node = NULL ; 
    }
    this->_node = node ; 
    if (this->_node){
        json_node_ref(this->_node); 
    }
    
}  

JsonNode *Response::getJsonNode() const{
    return this->_node ; 
} 

void Response::setResponse(const std::string &respMessage) {
    this->_respMessage = respMessage ; 
}
void Response::setResponse(const char *respMessage) {
    this->_respMessage = respMessage ; 
}
void Response::setResponse(const char *respMessage, const int messageLen) {
    if (respMessage == NULL || messageLen <= 0) return ; 
    this->_respMessage = std::string(respMessage, messageLen); 
}
void Response::setHeaderMap(const char *headerName, const char *headerValue) {
    this->_respHeader.insert(std::pair<std::string, std::string>(headerName, headerValue));
}

void Response::setExpiredSession(bool isSessionExpired) {
    this->_isSessionExpired = isSessionExpired;
}

const std::string &Response::getResponseMessage(){
    return this->_respMessage ; 
}  
const std::string Response::getResponseHeaderValue(const char *headerName) {
    std::string rv;
    auto it = this->_respHeader.find(headerName);
    if (it != this->_respHeader.end()) {
        rv = it->second;
    }
    return rv;
}

void Response::setUrl(const char *url){
    if (url == NULL) return ; 
    this->_url = url ; 
}
const std::string &Response::getUrl(){
    return this->_url; 
}

bool Response::isSuccess(){
    return ((this->_status) >= 200 && (this->_status) < 300) ; 
} 

bool Response::isExpiredSession(){
    return this->_isSessionExpired;
}

END_FTC_CORE

