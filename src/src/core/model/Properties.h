#ifndef _FTC_CORE_MODEL_PROPERTIES_H_
#define _FTC_CORE_MODEL_PROPERTIES_H_
#include <unordered_map>
#include <string>
#include <json-glib/json-glib.h>
#include "../macro.h"
#include "ValueObject.h"

BEGIN_FTC_CORE

class Properties : public BaseValue {
public : 
    Properties(); 
    virtual ~Properties(); 
protected : 
    virtual bool parseJsonNode(JsonNode* node) override; 
    virtual bool internalClear() override ; 
private : 
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(bool, _use_clipboard, UseClipboard);
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(bool, _use_url_redirection, UseUrlRedirection);
}; 

END_FTC_CORE 
#endif /* _FTC_CORE_MODEL_PROPERTIES_H_ */