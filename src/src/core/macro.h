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


#ifndef _FTC_CORE_MACRO_H_
#define _FTC_CORE_MACRO_H_

#define BEGIN_FTC \
    namespace Ftc \
    {
#define END_FTC }
#define BEGIN_FTC_CORE \
    namespace Ftc      \
    {                  \
    namespace Core     \
    {
#define END_FTC_CORE \
    }                \
    }

#define USING_FTC using namespace Ftc
#define USING_FTC_CORE using namespace Ftc::Core


#define FTC_SYNTHESIZE(varType, varName, funName)\
protected: varType varName; public: virtual inline varType get##funName() const { return varName; } virtual inline void set##funName(varType var){ varName = var; }

#define FTC_SYNTHESIZE_PASS_BY_REF(varType, varName, funName)\
protected: varType varName; public: virtual inline varType& get##funName() { return varName; } virtual inline void set##funName(const varType& var){ varName = var; }

#define FTC_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName; public: virtual inline varType get##funName() const { return varName; }

#define FTC_SYNTHESIZE_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName; public: virtual inline const varType& get##funName() const { return varName; }



#define HUNE_CALLBACK_OBJECT_0(__func__, __target__, ...) std::bind(&__func__, __target__, ##__VA_ARGS__)
#define HUNE_CALLBACK_OBJECT_1(__func__, __target__, ...) std::bind(&__func__, __target__, std::placeholders::_1, ##__VA_ARGS__)
#define HUNE_CALLBACK_OBJECT_2(__func__, __target__, ...) std::bind(&__func__, __target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)

#define HUNE_CALLBACK_CLS_STATIC_0(__func__, ...) std::bind(&__func__, ##__VA_ARGS__)
#define HUNE_CALLBACK_CLS_STATIC_1(__func__, ...) std::bind(&__func__, std::placeholders::_1, ##__VA_ARGS__)
#define HUNE_CALLBACK_CLS_STATIC_2(__func__, ...) std::bind(&__func__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)



#endif /* _FTC_CORE_MACRO_H_ */