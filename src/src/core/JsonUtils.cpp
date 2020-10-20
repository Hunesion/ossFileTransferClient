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

#include "JsonUtils.h"

BEGIN_FTC_CORE
namespace JsonUtils{
    JsonObject* getObject(JsonNode *node){
        JsonObject* rv = NULL ; 
        if (!node) return rv ; 
        rv = json_node_get_object(node); 
        return rv ; 
    }

    JsonArray* getArray(JsonNode *node){
        JsonArray *rv = NULL ; 
        if (!node) return rv ; 
        rv = json_node_get_array(node); 
        return rv ; 
    }

    JsonObject* getObject(JsonObject *obj, const char *name){
        JsonObject *rv = NULL ; 
        if (obj == NULL || name == NULL) return rv ; 
        rv = json_object_get_object_member(obj, name); 
        return rv ; 
    }

    JsonArray* getArray(JsonObject *obj , const char *name){
        JsonArray *rv = NULL ; 
        if (obj == NULL || name == NULL) return rv ; 
        rv = json_object_get_array_member(obj, name); 
        return rv ; 
    }

    const char* getString(JsonObject *obj , const char *name){
        const char *rv = NULL ; 
        if (obj == NULL || name == NULL) return rv ; 
        rv = json_object_get_string_member(obj, name); 
        return rv ;
    }

    std::string getStdString(JsonObject* obj , const char *name){
        std::string rv ; 
        const char *str = NULL ; 
        str = getString(obj, name); 
        if (str){
            rv = str ; 
        }
        return rv ; 
    }

    bool getBool(JsonObject *obj, const char *name , bool dv){
        bool rv = dv ; 
        if (obj == NULL || name == NULL) return rv ; 
        rv = json_object_get_boolean_member(obj, name); 
        return rv ; 
    }

    int getInt(JsonObject *obj, const char *name, int dv){
        int rv = dv ; 
        if (obj == NULL || name == NULL) return rv ; 
        rv = json_object_get_int_member(obj, name); 
        return rv ; 
    }

    static JsonNode *json_object_get_member_or_array_element(JsonObject *json , const char *name){
        JsonNode *rv = NULL ; 
        int array_begin = -1 , array_end = -1 , size = 0 , i = 0 ; 
        if (json == NULL || name == NULL) return rv ; 
        if ((size = strlen(name)) == 0) return rv ; 

        for (i = 0 ; i < size ; i++){
            if (name[i] == '['){
                array_begin = i ; 
            }

            if (name[i] == ']'){
                array_end = i ; 
            }
        }

        if (array_begin > 0 && array_end > 0 && (array_begin < array_end)){
            JsonArray *array = NULL ; 
            int end = array_begin + 1; 
            std::string attr (name,array_begin); 
            std::string str_indx (name + end, (array_end - end));
            int index = -1 ;
            try{
                index = std::stoi(str_indx) ; 
            }catch ( ... ){
                index = -1 ; 
            }

            if (index < 0) return rv ; 
            array = getArray(json, attr.c_str()); 
            if (!array || json_array_get_length(array) <= index) return rv ; 
            rv = json_array_get_element(array, index);
        }else {
            rv = json_object_get_member(json, name); 
        }


        return rv ; 
    }

    JsonNode *getNode(JsonNode* node, const char *name, int dep){
        JsonNode * rv = NULL ; 
        JsonObject *obj = NULL ; 
        int len = 0; 
        int offset = 0 ; 

        if (!node || !name) return rv ; 

        if (dep >= 10) return rv ;
        len = strlen(name); 

        obj = getObject(node); 
        if (!obj) return rv; 

        for (int i = 0 ; i < len ; i++){
            offset = i ; 
            if (name[i] == ':'){
                break ; 
            }
        }

        if (len == (offset + 1)){
            rv = json_object_get_member_or_array_element(obj, name); 
        }else {
            std::string attr ; 
            JsonNode* tmpNode = NULL ; 
            attr = std::string(name, offset);
            if (attr.empty()) return rv ; 
            tmpNode = json_object_get_member_or_array_element(obj, attr.c_str()); 
            if (tmpNode){
                name = name + (offset + 1); 
                rv = getNode(tmpNode, name, ++dep); 
            }
        }
        return rv ; 
    }

    const char* getString(JsonNode* node , const char *name){
        const char* rv = NULL ; 
        JsonNode *tmp = NULL ; 
        tmp = getNode(node, name, 0); 
        if (!tmp) return rv ; 
        rv = json_node_get_string(tmp); 
        return rv ; 
    }

    std::string getStdString(JsonNode* node , const char *name){
        std::string rv ; 
        JsonNode *tmp = NULL ; 
        tmp = getNode(node, name, 0); 
        if (!tmp) return rv ; 
        rv = json_node_get_string(tmp); 
        return rv ; 
    }

    bool getBool(JsonNode *node, const char *name , bool dv){
        bool rv = dv ; 
        JsonNode *tmp = NULL ; 
        tmp = getNode(node, name, 0); 
        if (!tmp) return rv ; 
        rv = json_node_get_boolean(tmp);  
        return rv ; 
    }

    int getInt(JsonNode *node, const char *name, int dv){
        int rv = dv ; 
        JsonNode *tmp = NULL ; 
        tmp = getNode(node, name, 0); 
        if (!tmp) return rv ; 
        rv = json_node_get_int(tmp);  
        return rv ; 
    }

    JsonObject* getObject(JsonNode *node, const char *name){
        JsonObject* rv = NULL ; 
        JsonNode *tmp = NULL ; 
        tmp = getNode(node, name, 0); 
        if (!tmp) return rv ; 
        rv = json_node_get_object(tmp);  
        return rv ; 
    }

    JsonArray* getArray(JsonNode *node, const char *name){
        JsonArray* rv = 0 ; 
        JsonNode *tmp = NULL ; 
        tmp = getNode(node, name, 0); 
        if (!tmp) return rv ; 
        rv = json_node_get_array(tmp);  
        return rv ;
    } 

    int getArraySize(JsonNode *node, const char *name){
        JsonArray *array = NULL ; 
        int rv = 0 ; 
        array = getArray(node, name); 
        if (array == NULL ) return rv ; 
        rv = json_array_get_length(array); 
        return rv ; 
    }

    JsonNode* getNode(JsonNode* node, const char *name){
        JsonNode *rv = NULL ; 
        rv = getNode(node, name, 0);
        return rv ;
    }


     JsonObject * createObject(){
         JsonObject *rv = NULL ;
         rv = json_object_new(); 
         return rv ; 
     }

    JsonObject *setString(JsonObject *obj , const char *key, const char* value){
        JsonNode *node = NULL ; 
        if (obj == NULL || key == NULL || value == NULL) return obj ; 

        node = json_node_new(JSON_NODE_VALUE) ; 
        json_node_set_string(node, value); 
        json_object_set_member(obj, key, node); 

        return obj ; 
    }

    JsonObject *setString(JsonObject *obj ,const std::string& key, const std::string& value){
        JsonNode *node = NULL ; 
        if (obj == NULL ||  key.empty() || value.empty() ) return obj ; 
        return setString(obj, key.c_str(), value.c_str()); 
    }


    JsonObject *setInt(JsonObject *obj , const char *key, const int value){
        JsonNode *node = NULL ; 
        if (obj == NULL || key == NULL ) return obj ; 

        node = json_node_new(JSON_NODE_VALUE) ; 
        json_node_set_int(node, value); 
        json_object_set_member(obj, key, node); 
        return obj ; 
    }
    JsonObject *setInt(JsonObject *obj ,const std::string& key, const int value){
        JsonNode *node = NULL ; 
        if (obj == NULL || key.empty() ) return obj ; 

        return setInt(obj, key.c_str(), value); 
    }

    JsonObject *setBoolean(JsonObject *obj , const char *key, const bool value){
        JsonNode *node = NULL ; 
        if (obj == NULL || key == NULL ) return obj ; 

        node = json_node_new(JSON_NODE_VALUE) ; 
        json_node_set_boolean(node, value); 
        json_object_set_member(obj, key, node); 
        return obj ; 
    } 

    JsonObject *setBoolean(JsonObject *obj ,const std::string& key, const bool value){
        JsonNode *node = NULL ; 
        if (obj == NULL || key.empty() ) return obj ; 
        return setBoolean(obj, key.c_str(), value); 
    }

    JsonObject *setObject(JsonObject *obj , const char *key, JsonObject *value){
        JsonNode *node = NULL ; 
        if (obj == NULL || key == NULL || value == NULL) return obj ; 
        json_object_set_object_member(obj, key, value);
        return obj; 
    }

    JsonObject *setObject(JsonObject *obj , const std::string& key, JsonObject *value){
        JsonNode *node = NULL ; 
        if (obj == NULL || key.empty() || value == NULL) return obj ; 
        return setObject(obj, key.c_str(), value); 
    }

    JsonObject *setArray(JsonObject *obj, const char *key, JsonArray *value){
        if (obj == NULL || key == NULL  || value == NULL) return obj ;
        json_object_set_array_member(obj, key, value);
        return obj ; 
    }

    JsonObject *setArray(JsonObject *obj, const std::string& key, JsonArray *value){
        if (obj == NULL || key.empty()  || value == NULL) return obj ;
        setArray(obj, key.c_str(), value); 
        return obj ;
    }

    JsonArray *createArray(){
        JsonArray *rv = NULL ; 
        rv = json_array_new(); 
        return rv ; 
    }


    JsonArray *addString(JsonArray *arr , const char *value){
        if (arr == NULL || value == NULL) return arr ;
        json_array_add_string_element(arr, value); 
        return arr ; 
    }
    JsonArray *addString(JsonArray *arr , const std::string& value){
        if (arr == NULL || value.empty()) return arr ;
        return addString(arr, value.c_str()); 
    }
    JsonArray *addObject(JsonArray *arr , JsonObject *obj){
        if (arr == NULL || obj == NULL) return arr;
        json_array_add_object_element(arr, obj);
        return arr;
    }

    JsonNode* objectToNode(JsonObject **obj){
        JsonNode *rv = NULL;

        if (!obj || !(*obj)) return rv ; 
        rv = json_node_new(JSON_NODE_OBJECT); 
        if (rv == NULL) return rv; 
        json_node_set_object(rv , *obj); 

        json_object_unref(*obj); 
        *obj = NULL ; 

        return rv;
    }

    std::string jsonToString(JsonObject **obj){
        std::string rv ; 
        char *str = NULL ;
        JsonNode* node = NULL ; 

        if (!obj || !(*obj)) return rv ; 

        node = json_node_new(JSON_NODE_OBJECT); 
        if (node == NULL) return rv; 
        json_node_set_object(node , *obj); 

        str = json_to_string(node, false); 
        if (str){
            rv = str ; 
        }


        if (str){
            g_free(str); 
            str = NULL ; 
        }

        json_object_unref(*obj); 
        *obj = NULL ; 

        if (node){
            json_node_free(node); 
            node = NULL ; 
        }

        return rv ; 

    }

}
END_FTC_CORE