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

#ifndef _FTC_CORE_FILE_INFO_H_
#define _FTC_CORE_FILE_INFO_H_
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include "macro.h"
BEGIN_FTC_CORE
class FileInfo{
private:
    FileInfo(const char *path); 
public:
    FileInfo(); 
    virtual ~FileInfo(); 
    static FileInfo getFileInfo(const char *path); 
    bool isExist() const ; 
    bool isFile() const ; 
    bool isDirectory() const ; 
    bool isReadable() const ; 
    bool isWritable() const ; 
    long long length() const ;
    const std::string &getFullPath() const;  
    const std::string &getPath() const ; 
    const std::string &getFileName() const ; 
    const std::string &getRelativeDir() const ;
    void setRelativeDir(const char *relativeDir);
private: 
    void seperatePath(); 

private: 
    std::string _fullPath ; 
    std::string _fileName ; 
    std::string _path ; 
    std::string _relativeDir ;
    struct stat _stat ; 
    bool _exist ; 
}; 
END_FTC_CORE
#endif