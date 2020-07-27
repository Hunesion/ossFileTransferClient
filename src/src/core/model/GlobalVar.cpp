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

#include <string>
#include "GlobalVar.h"
#include "../macro.h"
#include "../JsonUtils.h"

BEGIN_FTC_CORE

static ClientConfig s_ClientConfig ; 

namespace GlobalVar
{
    void init(){
        s_ClientConfig.init(); 
    }

    ClientConfig &getClientConfig(){
        return s_ClientConfig; 
    }

    const char *getFtcUrl() {
        return s_ClientConfig.getFtcUrl().c_str(); 
    }

    void dispose() {
        s_ClientConfig.dispose();
    }

} // namespace GlobalVar
END_FTC_CORE
