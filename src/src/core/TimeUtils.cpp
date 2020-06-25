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


#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <cstdarg>

#include "TimeUtils.h"
#include "StringUtils.h"

BEGIN_FTC_CORE

namespace TimeUtils
{
    std::tm now()
    {
        std::tm rv;
        std::time_t now_date = std::time(NULL);
        localtime_r(&now_date, &rv);
        return rv;
    }

    std::tm addDay(std::tm &tm, int day)
    {
        std::tm rv;
        std::time_t time = std::mktime(&tm);
        int day_abs = abs(day);

        if (day > 0) {
            time += 60 * 60 * 24 * day_abs;
        } else {
            time -= 60 * 60 * 24 * day_abs;
        }
        
        localtime_r(&time, &rv);
        return rv;
    }

    std::string getDateFormat(std::tm &tm, const char *format)
    {
        char buffer[256] = { 0, };

        if (! format) {
            return "";
        }

        strftime(buffer, 256, format, &tm);
        
        return buffer;
    }

    std::tm dateFormatToTm(const char *date/*yyyymmdd*/)
    {
        
    }
} // namespace TimeUtils

END_FTC_CORE