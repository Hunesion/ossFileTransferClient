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

#include "FileUtils.h"
#include "Logger.h"
#include "StringUtils.h"
#include "FileInfo.h"
#include <sstream>



BEGIN_FTC_CORE
namespace FileUtils
{
        
    bool fileWrite(const char *filepath, const char *buffer, unsigned int len, int mode) {
        GFile *file = NULL;
        GOutputStream *stream = NULL;
        GError *error = NULL;
        gsize outLen = 0, writeBytes = 0;
        bool rv = false, exist = false;

        if (filepath == NULL || buffer == NULL) {
            return rv;
        }
        if (len <= 0) {
            return rv;
        }

        file = g_file_new_for_path(filepath);
        if (file == NULL) {
            return rv;
        }

        if (g_file_query_exists(file, NULL) == true) {
            if (mode == 0) {
                return rv;
            } else if (mode == 1) {
                if (g_file_delete(file, NULL, NULL) == false) {
                    return rv;
                }
            } else {
                exist = true;
            }
        }

        try {
            if (exist) {
                stream = G_OUTPUT_STREAM(g_file_append_to(file, G_FILE_CREATE_NONE, NULL, &error));
            } else {
                stream = G_OUTPUT_STREAM(g_file_create(file, G_FILE_CREATE_NONE, NULL, &error));
            }
            if (stream == NULL) {
                throw 1;
            }

            if (g_output_stream_write_all(stream, buffer, len, &outLen, NULL, &error) == false) {
                throw 2;
            }

            rv = true;
        } catch (int exception) {
            FTC_LOG("FileUtis File Write Exception %d", exception);
        }

        if (stream != NULL) {
            g_output_stream_close(stream, NULL, NULL);
            g_object_unref(stream);
            stream = NULL;
        }

        if (file != NULL) {
            g_object_unref(file);
            file = NULL;
        }

        if (error != NULL) {
            g_error_free(error);
            error = NULL;
        }

        return rv;
    }

    bool fileDelete(const char *filepath)
    {
        GFile *file = NULL;
        bool rv = false;

        if (filepath == NULL) {
            return rv;
        }

        file = g_file_new_for_path(filepath);
        if (file == NULL) {
            return rv;
        }

        if (g_file_query_exists(file, NULL) == true) {
            rv = g_file_delete(file, NULL, NULL);
        }

        return rv;
    }

    std::string fileReadString(const char *filepath)
    {
        GFile *file = NULL;
        GInputStream *istream = NULL;
        GError *error = NULL;
        gsize readBytes = 0;
        std::string rv = "";
        char buffer[1024] = { 0, };

        if (! filepath) {
            return rv;
        }

        file = g_file_new_for_path(filepath);
        if (! file) {
            return rv;
        }

        try
        {
            istream = G_INPUT_STREAM(g_file_read(file, NULL, &error));
            if (! istream) {
                
                throw std::runtime_error(error->message);
            }

            do
            {
                readBytes = g_input_stream_read(istream, buffer, sizeof(buffer), NULL, &error);
                if (readBytes <= 0) {
                    break;
                }
                rv.append(buffer, readBytes);
            } while (true);
        }
        catch(const std::runtime_error& e)
        {
            FTC_LOG("FileUtils File Read String exception %s \n", e.what());
        }

        if (istream != NULL) {
            g_input_stream_close(istream, NULL, NULL);
            g_object_unref(istream);
            istream = NULL;
        }

        if (file != NULL) {
            g_object_unref(file);
            file = NULL;
        }

        if (error != NULL) {
            g_error_free(error);
            error = NULL;
        }

        return rv;
    }

    bool createDirIfnotExist(const char *dirpath)
    {
        GFile *file = NULL;
        GError *error = NULL;
        bool rv = false;
        
        file = g_file_new_for_path(dirpath);
        if (file == NULL) {
            return rv;
        }    

        if (g_file_query_exists(file, NULL) == true) {
            rv = true;
        } else {
            if (g_file_make_directory_with_parents(file, NULL, &error) == true) {
                rv = true;
            } else {
                FTC_LOG("createDirIfnotExist FAIL %s", error->message);
            }
        }
        
        if (file != NULL) {
            g_object_unref(file);
            file = NULL;
        }

        if (error != NULL) {
            g_error_free(error);
            error = NULL;
        }

        return rv;
    }

    bool isFileExist(const char *filepath) 
    {
        GFile *file = NULL;
        bool rv = false;
        
        file = g_file_new_for_path(filepath);
        if (file == NULL) {
            return rv;
        }    

        if (g_file_query_exists(file, NULL) == true) {
            rv = true;
        }

        if (file != NULL) {
            g_object_unref(file);
            file = NULL;
        }

        return rv;
    }

    std::string getFileExtension(const char *filepath)
    {
        std::string rv = "", file_name;
        std::string::size_type idx;

        if (!filepath) {
            return rv;
        }

        file_name = filepath;

        idx = std::string(file_name).rfind('.');
        if (idx != std::string::npos) {
            rv = file_name.substr(idx + 1);
        }
        
        return rv;
    }

    std::string getFileNameUniqueIndex(const char *filepath)
    {
        FileInfo fi = FileInfo::getFileInfo(filepath);
        std::string rv, indexFilename, filename, extension, dir, indexFullpath;
        int index = 1;

        if (fi.isExist() == false) {
            rv = fi.getFileName();
            return rv;
        }

        dir = fi.getPath();
        filename = fi.getFileName();
        extension = "." + getFileExtension(filename.c_str());
        if (StringUtils::endsWith(filename, extension.c_str())) {
            filename = filename.substr(0, filename.length() - extension.length());
        }
        
        
        do {
            indexFilename = filename;
            indexFilename.append("(");
            indexFilename.append(std::to_string(index));
            indexFilename.append(")");
            indexFilename.append(extension);

            indexFullpath = dir + "/" + indexFilename;
            fi = FileInfo::getFileInfo(indexFullpath.c_str());
            index++;
        } while (fi.isExist() == true);

        rv = fi.getFileName();
        return rv;
    }
}
END_FTC_CORE
