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

#include <map>
#include <string>
#include <pwd.h>
#include <sstream>
#include "../StringUtils.h"
#include "ValueObject.h"
#include "ClientConfig.h"
#include "../Logger.h"
#include "../FileInfo.h"
#include "../FileUtils.h"

BEGIN_FTC_CORE

ClientConfig::ClientConfig(){
    
}

ClientConfig::~ClientConfig(){
    
}

void ClientConfig::init(){
    this->gschemaRead();
    this->configRead();
}

void ClientConfig::dispose() {
    this->configWrite();
    this->gschemaWrite();
}

bool ClientConfig::gschemaRead() {
    GSettings *settings = nullptr;
    gchar *value = nullptr;
    bool rv = false;

    settings = g_settings_new(FTC_GSCHEMA_ID);
    FTC_LOG("gschema open for read, gschema id : %s", FTC_GSCHEMA_ID);
    if (! settings) {
        return rv;
    }
    
    value = g_settings_get_string(settings, FTC_URL);
    if (value) {
        this->_ftcUrl = value;
        
        g_free(value);
        value = nullptr;
    }

    value = g_settings_get_string(settings, FTC_SERVER);
    if (value) {
        this->_ftcServer = value;

        g_free(value);
        value = nullptr;
    }

    value = g_settings_get_string(settings, CLIPBOARD_PORT);
    if (value) {
        this->_clipboardPort = StringUtils::getInt(value);
        g_free(value);
        value = nullptr;
    }

    value = g_settings_get_string(settings, FILE_DOWNLOAD_THREADCNT);
    if (value) {
        this->_fileDownloadThreadCnt = StringUtils::getInt(value);

        g_free(value);
        value = nullptr;
    }

    value = g_settings_get_string(settings, FILE_UPLOAD_THREADCNT);
    if (value) {
        this->_fileUploadThreadCnt = StringUtils::getInt(value);

        g_free(value);
        value = nullptr;
    }

    rv = true;

    if (value) {
        g_free(value);
        value = nullptr;
    }

    if (settings) {
        g_object_unref(settings);
        settings = nullptr;
    }

    return rv;
}

bool ClientConfig::gschemaWrite() {
    GSettings *settings = nullptr;
    std::string value;
    bool rv = false;

    settings = g_settings_new(FTC_GSCHEMA_ID);
    FTC_LOG("gschema open for write, gschema id : %s", FTC_GSCHEMA_ID);
    if (! settings) {
        return rv;
    }

    g_settings_set_string(settings, FTC_URL, this->_ftcUrl.c_str());
    g_settings_set_string(settings, FTC_SERVER, this->_ftcServer.c_str());

    value = std::to_string(this->_clipboardPort);
    g_settings_set_string(settings, CLIPBOARD_PORT, value.c_str());

    value = std::to_string(this->_fileDownloadThreadCnt);
    g_settings_set_string(settings, FILE_DOWNLOAD_THREADCNT, value.c_str());

    value = std::to_string(this->_fileUploadThreadCnt);
    g_settings_set_string(settings, FILE_UPLOAD_THREADCNT, value.c_str());

    rv = true;

    if (settings) {
        g_object_unref(settings);
        settings = nullptr;
    }

    return rv;
}

bool ClientConfig::configRead() {
    GKeyFile *keyfile = nullptr;
    char *value = nullptr;
    bool rv = false;

    keyfile = this->loadConfigFile();

    if (! keyfile) {
        return rv;
    }

    value = g_key_file_get_string(keyfile, "USER", "userid", nullptr);
    if (value) {
        this->_userId = value;
        g_free(value);
        value = nullptr;
    }

END:
    if (keyfile) {
        g_key_file_free(keyfile);
        keyfile = nullptr;
    }

    if (value) {
        g_free(value);
        value = nullptr;
    }

    return rv;
}

bool ClientConfig::configWrite() {
    GKeyFile *keyfile = nullptr;
    GError *error = nullptr;
    bool rv = false;

    keyfile = this->loadConfigFile();

    if (! keyfile) {
        return rv;
    }

    g_key_file_set_string(keyfile, "USER", "userid", this->_userId.c_str());

    if (g_key_file_save_to_file(keyfile, getConfigFilePath().c_str(), &error) == false) {
        FTC_LOG("fail to save config file : %s", error->message);
    }

    if (keyfile) {
        g_key_file_free(keyfile);
        keyfile = nullptr;
    }

    if (error) {
        g_object_unref(error);
        error = nullptr;
    }

    return rv;
}

std::string ClientConfig::getConfigFilePath() {
    std::stringstream rv;
    
    rv << this->getConfigDirPath();
    rv << "/config.ini";

    return rv.str();
}

std::string ClientConfig::getConfigDirPath() const {
    std::string homeDir;
    std::stringstream rv;
    const passwd *pwd = nullptr;

    homeDir = this->getUserHomeDirPath();

    rv << homeDir;
    rv << "/.ftc";

    return rv.str();
}

std::string ClientConfig::getUserDownloadDirPath() const {
    std::string rv, homeDir;
    std::stringstream ssEng, ssKor;
    passwd *pwd = nullptr;
    GFile *downDirEng = nullptr, *downDirKor = nullptr;

    homeDir = this->getUserHomeDirPath();

    ssEng << homeDir;
    ssEng << "/Downloads";
    downDirEng = g_file_new_for_path(ssEng.str().c_str());
    if (downDirEng) {
        if (g_file_query_exists(downDirEng, nullptr)) {
            rv = ssEng.str();
        }
    }

    if (rv.length() <= 0) {
        ssKor << homeDir;
        ssKor << "/다운로드";
        downDirKor = g_file_new_for_path(ssKor.str().c_str());
        if (downDirKor) {
            if (g_file_query_exists(downDirKor, nullptr)) {
                rv = ssKor.str();
            }
        }
    }

END:
    if (downDirEng) {
        g_object_unref(downDirEng);
        downDirEng = nullptr;
    }

    if (downDirKor) {
        g_object_unref(downDirKor);
        downDirKor = nullptr;
    }

    return rv;
}

std::string ClientConfig::getUserHomeDirPath() const {
    std::string rv;
    passwd *pwd = nullptr;
    const char *homeDirEnv = nullptr;
    GFile *homeDir = nullptr;

    homeDirEnv = getenv("HOME");
    if (! homeDirEnv) {
        pwd = getpwuid(getuid());
        if (! pwd) {
            goto END;
        }

        homeDirEnv = pwd->pw_dir;
    }

    homeDir = g_file_new_for_path(homeDirEnv);
    if (! homeDir) {
        goto END;
    }

    if (! g_file_query_exists(homeDir, nullptr)) {
        goto END;
    }
    
    rv = homeDirEnv; 
END:
    if (homeDir) {
        g_object_unref(homeDir);
        homeDir = nullptr;
    }

    return rv;
}

GKeyFile* ClientConfig::loadConfigFile() {
    GKeyFile *rv = nullptr;
    GError *error = nullptr;
    FileInfo fi;
    std::string path;
    bool result = true;
    
    rv = g_key_file_new();
    if (! rv) {
        return rv;
    }

    path = this->getConfigFilePath();
    fi = FileInfo::getFileInfo(path.c_str());

    if (fi.isExist() == false) {
        result = this->existConfigDir();
    } else {
        result = g_key_file_load_from_file(rv, path.c_str(), GKeyFileFlags::G_KEY_FILE_KEEP_COMMENTS, &error);
    }
    
    if (! result) {
        g_key_file_free(rv);
        rv = nullptr;

        if (error) {
            FTC_LOG("g_key_file_load_from_file fail : %s", error->message);
            g_object_unref(error);
        }
    }

    return rv;
}

bool ClientConfig::existConfigDir() const {
    GFile *dir = nullptr;
    bool rv = false;
    std::string path = this->getConfigDirPath();

    dir = g_file_new_for_path(path.c_str());
    if (dir) {
        if (g_file_query_exists(dir, nullptr) == true) {
            rv = true;
        } else {
            if (g_file_make_directory_with_parents(dir, nullptr, nullptr) == true) {
                rv = true;
            }
        }
    }

    if (dir) {
        g_object_unref(dir);
        dir = nullptr;
    }

    return rv;
}
END_FTC_CORE
