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


#ifndef _FTC_CORE_TIME_UTILS_H_
#define _FTC_CORE_TIME_UTILS_H_
#include "StringUtils.h"
#include "macro.h"
#include <ctime>

BEGIN_FTC_CORE
namespace TimeUtils {
    std::tm now();
    std::tm addDay(std::tm &tm, int day);
    std::string getDateFormat(std::tm &tm, const char *format);
    std::tm dateFormatToTm(const char *date/*yyyymmdd*/);
}
END_FTC_CORE
#endif /* _FTC_CORE_TIME_UTILS_H_ */
