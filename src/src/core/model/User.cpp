#include "../JsonUtils.h"
#include "../StringUtils.h"
#include "User.h"


BEGIN_FTC_CORE
User::User():BaseValue(), _obj(nullptr){
}
User::~User(){
    this->clear(); 
}
bool User::parseJsonNode(JsonNode *node) {
    bool rv = false ; 
    JsonObject *obj = nullptr ; 
    std::string tmp_str;
    if (!node) return rv ; 
    obj = JsonUtils::getObject(node); 
    if (!obj) return rv ; 
    this->_result = JsonUtils::getStdString(obj, RESULT_KEY);


    if (this->_result.compare(RESULT_SUCCESS) != 0) {
        return rv ; 
    }
    this->_obj = obj ; 

    this->_contact = JsonUtils::getStdString(this->_obj, "contact");
    this->_userid = JsonUtils::getStdString(this->_obj, "userid");
    this->_network_id = JsonUtils::getStdString(this->_obj, "network_id");
    tmp_str = JsonUtils::getStdString(this->_obj, "formats_allow_list");
    this->_formats_allow_list = StringUtils::splitToVector(tmp_str, ',');

    rv = true ; 
    return rv ; 
}  
bool User::internalClear() {
    this->_obj = nullptr ; 
    return true ; 
}

const JsonObject *User::getJsonObject() const{
    return this->_obj; 
}
END_FTC_CORE