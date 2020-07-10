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