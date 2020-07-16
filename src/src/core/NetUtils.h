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

#ifndef _FTC_CORE_NET_UTILS_H_
#define _FTC_CORE_NET_UTILS_H_

#include "macro.h"
#include <list>
#include <string>

typedef struct _NetInfo{
    std::string ifname ; 
    std::string macAddr ; 
    std::string inetAddr ; 
} NetInfo ; 

BEGIN_FTC_CORE
namespace NetUtils{
std::list<NetInfo> getNetInfo(); 
}
END_FTC_CORE


#endif