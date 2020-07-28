#ifndef _FTC_CORE_MODEL_USER_H_
#define _FTC_CORE_MODEL_USER_H_
#include "../macro.h"
#include "ValueObject.h"

BEGIN_FTC_CORE

class User : public BaseValue{
public : 
    User() ; 
    virtual ~User(); 
    const JsonObject *getJsonObject() const ; 
protected :
    virtual bool parseJsonNode(JsonNode *node) override ; 
    virtual bool internalClear() override ; 

    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string , _result, Result);
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string , _contact, Contact); 
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string , _userid, UserId); 
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string , _network_id, NetworkId); 
private: 
    JsonObject *_obj ; 
}; 
END_FTC_CORE


#endif /* _FTC_CORE_MODEL_USER_H_ */
