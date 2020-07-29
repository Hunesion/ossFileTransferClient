#include <iostream>
#include "../StringUtils.h"
#include "../JsonUtils.h"
#include "Properties.h"


BEGIN_FTC_CORE
Properties::Properties():BaseValue(){
}

Properties::~Properties(){
    this->clear(); 
}

bool Properties::parseJsonNode(JsonNode *node){
    bool rv = false ; 
    JsonObject *jobj = nullptr ;
    JsonNode *jtmp = nullptr ; 
    JsonArray *list = nullptr ;  
    const char *result = nullptr ; 
    const char *enc_key = nullptr ;
    std::string enc ; 
    size_t offset =  std::string::npos ; 
    if (!node) {
        return rv; 
    }
    
    jobj = JsonUtils::getObject(node); 
    if (!jobj) {
        return rv ; 
    }

    result = JsonUtils::getString(jobj, "result"); 
    if (!result) {
        return rv ; 
    }
    if (strncmp(result, "success", 7) != 0) {
        return rv ; 
    }

    rv = true ; 
    return rv ; 
}

bool Properties::internalClear(){
    bool rv = false ; 
    rv = true ; 
    return rv ; 
}

END_FTC_CORE