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

#ifndef _FTC_CORE_FILE_UTILS_H_
#define _FTC_CORE_FILE_UTILS_H_
#include "macro.h"
#include <gtk/gtk.h>
#include <string>

BEGIN_FTC_CORE
namespace FileUtils
{
    bool fileWrite(const char *filepath, const char *buffer, unsigned int len, int mode = 0);

    bool fileDelete(const char *filepath);

    std::string fileReadString(const char *filepath);

    bool createDirIfnotExist(const char *dirpath);

    bool isFileExist(const char *filepath);

    std::string getFileExtension(const char *filepath);

    std::string getFileNameUniqueIndex(const char *filepath);
}
END_FTC_CORE

#endif /*_FTC_CORE_FILE_UTILS_H_*/