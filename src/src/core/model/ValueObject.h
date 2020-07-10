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

END_FTC_CORE
#endif /* _FTC_CORE_MODEL_VALUE_OBJECT_H_ */