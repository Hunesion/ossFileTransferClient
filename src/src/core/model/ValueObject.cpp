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

#include "../JsonUtils.h"
#include "../StringUtils.h"
#include "ValueObject.h"


BEGIN_FTC_CORE

static std::string s_empty = ""; 

const std::string& getEmptyString(){
    return s_empty; 
}

BaseValue::BaseValue():_node(nullptr){

}

BaseValue::BaseValue(const BaseValue &b){
    this->_node = b._node ; 
    if (this->_node){
        json_node_ref(this->_node); 
    }
}

BaseValue::~BaseValue(){
    if (this->_node){
        json_node_unref(this->_node); 
        this->_node = nullptr ; 
    }
}

bool BaseValue::setJosnNode(JsonNode *node){
    bool rv = false ; 
    if (!node) return rv ; 
    this->clear(); 
    if (this->parseJsonNode(node)){
        this->_node = node ; 
        json_node_ref(this->_node); 
        rv = true ; 
    }
    return rv ; 
}

JsonNode* BaseValue::getJsonNode() const {
    return this->_node ; 
}

void BaseValue::clear(){
    this->internalClear(); 
    if (this->_node){
        json_node_unref(this->_node); 
        this->_node = nullptr ; 
    }
}

/**
 * Begin Login Location  
 **/
LoginLocation::LoginLocation():BaseValue(){
}
LoginLocation::~LoginLocation(){
    this->clear(); 
}
bool LoginLocation::parseJsonNode(JsonNode *node)  {
    bool rv = false ; 
    JsonObject *jobj = NULL;
    if (!node) return rv ; 

    jobj = JsonUtils::getObject(node); 
    if (!jobj) {
        return rv ; 
    }

    if (JsonUtils::getStdString(node, RESULT_KEY).compare(RESULT_SUCCESS) != 0) {
        this->_error_msg = JsonUtils::getStdString(node, "message");
        return rv ; 
    } 

    this->_location = JsonUtils::getStdString(node, "location"); 
    this->_name = JsonUtils::getStdString(node, "name");
    this->_network_id = JsonUtils::getStdString(node, "network_id");

    if (this->_name.empty() || this->_location.empty()) return rv ; 

    rv = true ; 
    return rv ;
}
bool LoginLocation::internalClear()  {
    this->_location.clear(); 
    this->_name.clear();
    this->_network_id.clear();
    this->_error_msg.clear();
    return true ; 
}
/**
 * End Login Location 
 **/ 


/**
 * Begin LeftMenuCount
 **/
LeftMenuCount::LeftMenuCount():BaseValue(){

}

LeftMenuCount::~LeftMenuCount(){
    this->clear(); 
}

bool LeftMenuCount::parseJsonNode(JsonNode *node) {
    bool rv = false ; 
    JsonObject *obj = nullptr ; 
    JsonArray *arr = nullptr;
    int length = 0;
    const char *str = nullptr ; 
    obj = JsonUtils::getObject(node); 
    if (!obj) return rv ; 

    if (JsonUtils::getStdString(obj, "result") != "success") {
        return rv;
    }

    this->_receiveCnt = JsonUtils::getInt(obj, "receiveCnt", -1);
    this->_sendCnt = JsonUtils::getInt(obj, "sendCnt", -1);

    rv = true ; 
    return rv ; 
}

bool LeftMenuCount::internalClear() {
    this->_receiveCnt = -1;
    this->_sendCnt = -1;

}

END_FTC_CORE
