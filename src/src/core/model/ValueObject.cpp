
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
