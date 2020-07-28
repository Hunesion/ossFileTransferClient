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

#ifndef _FTC_CORE_MODEL_VALUE_OBJECT_H_
#define _FTC_CORE_MODEL_VALUE_OBJECT_H_
#include <string>
#include <unordered_map>
#include <vector>
#include <json-glib/json-glib.h>
#include "../macro.h"
#define RESULT_KEY "result"
#define RESULT_SUCCESS "success"

BEGIN_FTC_CORE


const std::string& getEmptyString(); 

class BaseValue{
protected:
    BaseValue(); 
    BaseValue(const BaseValue &b); 
    virtual ~BaseValue(); 
    virtual bool parseJsonNode(JsonNode *node) = 0 ; 
    virtual bool internalClear() = 0 ; 
public: 
    bool setJosnNode(JsonNode *node);
    JsonNode* getJsonNode() const; 
    void clear() ;
    bool isValid() const { return this->_node != nullptr ; } 

protected:
    JsonNode *_node ; 
}; 

class LoginLocation : public BaseValue{
public:
    LoginLocation(); 
    virtual ~LoginLocation(); 
    const JsonObject* getNetwork() const ;

protected :
    virtual bool parseJsonNode(JsonNode *node) override ; 
    virtual bool internalClear() override ; 

    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string , _location, Location); 
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string , _name, Name);
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string , _network_id, NetworkId);
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string , _error_msg, ErrorMsg);
private :  
}; 


END_FTC_CORE
#endif /* _FTC_CORE_MODEL_VALUE_OBJECT_H_ */