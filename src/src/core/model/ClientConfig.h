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

#ifndef _FTC_CORE_MODEL_CLIENT_CONFIG_H_
#define _FTC_CORE_MODEL_CLIENT_CONFIG_H_
#include <string>
#include <map>
#include "../macro.h"

/**
 * FTC C# Client 환경 설정 파일 정보
 **/
#define FTC_GSCHEMA_ID  "org.hunesion.app"

//  GSETTING KEY
#define FTC_URL "ftcurl"
#define FTC_SERVER "ftcserver"
#define CLIPBOARD_PORT "clipboardport"
#define FILE_UPLOAD_THREADCNT  "fileuploadthreadcnt"
#define FILE_DOWNLOAD_THREADCNT  "filedownloadthreadcnt"




BEGIN_FTC_CORE
class  ClientConfig
{
public:
    ClientConfig(); 
    virtual ~ClientConfig(); 

    void init(); 
    void dispose();

    std::string getConfigDirPath() const;
    std::string getUserDownloadDirPath() const;
    std::string getUserHomeDirPath() const;

    bool existConfigDir() const;
private:
    bool gschemaRead();
    bool gschemaWrite();
    bool configRead();
    bool configWrite();

    GKeyFile* loadConfigFile();
    std::string getConfigFilePath();
    

private:
    /*Global Schema Value*/
    FTC_SYNTHESIZE_PASS_BY_REF(std::string , _ftcUrl, FtcUrl); 
    FTC_SYNTHESIZE_PASS_BY_REF(std::string , _ftcServer, FtcServer); 
    FTC_SYNTHESIZE_PASS_BY_REF(int , _clipboardPort, ClipboardPort); 
    FTC_SYNTHESIZE_PASS_BY_REF(int , _fileDownloadThreadCnt, FileDownloadThreadCnt); 
    FTC_SYNTHESIZE_PASS_BY_REF(int , _fileUploadThreadCnt, FileUploadThreadCnt); 

    /*User Config Value*/
    FTC_SYNTHESIZE_PASS_BY_REF(std::string , _userId, UserId); 
}; 
END_FTC_CORE
#endif 