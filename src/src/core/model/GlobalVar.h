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

#ifndef _FTC_CORE_MODEL_GLOBAL_VAR_H_
#define _FTC_CORE_MODEL_GLOBAL_VAR_H_
#include <string>
#include <json-glib/json-glib.h>

#include "ClientConfig.h"
#include "ValueObject.h"


BEGIN_FTC_CORE
namespace GlobalVar
{
    void init(); 
    void setFtcUrl(const char *url);
    const char *getFtcUrl();
    ClientConfig &getClientConfig(); 
    void dispose();
} // namespace GlobalVar

END_FTC_CORE

#endif /* _FTC_CORE_MODEL_GLOBAL_VAR_H_ */