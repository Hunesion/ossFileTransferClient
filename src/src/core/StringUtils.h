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


#ifndef _FTC_CORE_STRING_UTILS_H_
#define _FTC_CORE_STRING_UTILS_H_
#include <list>
#include <vector>
#include <string>
#include "macro.h"
BEGIN_FTC_CORE
namespace StringUtils {
    void ltrim(std::string &s, char remove=' ');
    void rtrim(std::string &s, char remove=' ');
    void trim(std::string &s, char remove=' ');
    int getInt(const std::string &s); 
    int getInt(const char* s); 
    bool getBool(const std::string &s); 
    bool getBool(const char* s); 
    void replace(std::string &s, const std::string &p, const std::string &r);
    std::list<std::string> split(std::string &s, const char p);
    std::vector<std::string> splitToVector(std::string &s, const char p);
    void format(std::string &s, const std::string fmt, ...);
    std::string format(const std::string fmt, ...); 
    bool endsWith(const std::string& str, const char* suffix, unsigned suffixLen); 
    bool endsWith(const std::string& str, const char* suffix); 
    bool startsWith(const std::string& str, const char* prefix, unsigned prefixLen); 
    bool startsWith(const std::string& str, const char* prefix); 
}
END_FTC_CORE
#endif /* _FTC_CORE_STRING_UTILS_H_ */
