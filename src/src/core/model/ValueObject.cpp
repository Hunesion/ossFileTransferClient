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
END_FTC_CORE
