#ifndef _FTC_CORE_JSON_UTILS_H_
#define _FTC_CORE_JSON_UTILS_H_
#include <string>
#include <json-glib/json-glib.h>
#include "macro.h"

BEGIN_FTC_CORE
namespace JsonUtils{

    JsonObject* getObject(JsonNode *node);
    JsonArray* getArray(JsonNode *node);  
    JsonObject* getObject(JsonObject *obj, const char *name);
    JsonArray* getArray(JsonObject* obj , const char *name); 
    const char* getString(JsonObject* obj , const char *name); 
    std::string getStdString(JsonObject* obj , const char *name); 
    bool getBool(JsonObject *obj, const char *name , bool dv = false); 
    int getInt(JsonObject *obj, const char *name , int dv = 0); 


    const char* getString(JsonNode* node , const char *name); 
    std::string getStdString(JsonNode* node , const char *name); 
    bool getBool(JsonNode *node, const char *name , bool dv = false); 
    int getInt(JsonNode *node, const char *name , int dv = 0); 
    JsonObject* getObject(JsonNode *node, const char *name); 
    JsonArray* getArray(JsonNode *node, const char *name); 
    int getArraySize(JsonNode *node, const char *name); 
    JsonNode* getNode(JsonNode* node, const char *name); 


    JsonObject *createObject(); 
    JsonObject *setString(JsonObject *obj , const char *key, const char* value); 
    JsonObject *setString(JsonObject *obj ,const std::string& key, const std::string& value); 
    JsonObject *setInt(JsonObject *obj , const char *key, const int value); 
    JsonObject *setInt(JsonObject *obj ,const std::string& key, const int value); 
    JsonObject *setBoolean(JsonObject *obj , const char *key, const bool value); 
    JsonObject *setBoolean(JsonObject *obj ,const std::string& key, const bool value);
    JsonObject *setObject(JsonObject *obj ,const char *key, JsonObject *value);
    JsonObject *setObject(JsonObject *obj ,const std::string& key, JsonObject *value);
    JsonObject *setArray(JsonObject *obj, const char *key, JsonArray *value); 
    JsonObject *setArray(JsonObject *obj, const std::string& key, JsonArray *value);

    JsonArray *createArray() ; 
    JsonArray *addString(JsonArray *arr , const char *value); 
    JsonArray *addString(JsonArray *arr , const std::string& value);
    JsonArray *addObject(JsonArray *arr , JsonObject *obj);

    JsonNode* objectToNode(JsonObject **obj);
    std::string jsonToString(JsonObject **obj); 
    


}
END_FTC_CORE

#endif /* _FTC_CORE_JSON_UTILS_H_ */