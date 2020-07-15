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

#ifndef _FTC_CORE_LOGGER_H_
#define _FTC_CORE_LOGGER_H_

#include <stdarg.h>
#include "macro.h"

#define FTC_LOG(format, ... ) Ftc::Core::Logger::log(format , ##__VA_ARGS__)

#ifdef FTC_DEBUG
#define FTC_LOG_DEBUG(format , ... )  Ftc::Core::Logger::log(format , ##__VA_ARGS__)
#else 
#define FTC_LOG_DEBUG( ... ) do{}while(0)
#endif

BEGIN_FTC_CORE
namespace Logger{
    void log(const char *format , ... ); 
}
END_FTC_CORE


#endif