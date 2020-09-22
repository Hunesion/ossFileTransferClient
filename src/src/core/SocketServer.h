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

#ifndef __FTC_CORE_SOCKET_SERVER_H__
#define __FTC_CORE_SOCKET_SERVER_H__
#include <gio/gio.h>
#include <vector>
#include <list>
#include <string>
#include "macro.h"

BEGIN_FTC_CORE



class SocketServer{

private : 
    typedef struct _Socket {
        GSocketService *service;
        GSocketAddress *listening_addr;  
        int port;

        _Socket() {
            service = NULL;
            listening_addr = NULL;
            port = 0;
        }
    } Socket;
public:
    static SocketServer *getInstance(); 
    static void destroyInstance(); 
    
    bool start();
    bool work(std::string data);

    virtual ~SocketServer(); 
private: 
    SocketServer();
    
    bool start(int port);
    bool stop();  

    
    bool clipboard(std::string &data, std::vector<std::string> &splitData);
    bool urlRedirection(std::string &data, std::vector<std::string> &splitData);
    bool autoDownload(std::string &data, std::vector<std::string> &splitData);

    std::list<std::string> downloadFile(const std::string &requestInfoUid, const std::string &localIp, const std::string &dir);

private: 
    static SocketServer *s_instance ; 
    static volatile bool s_stop ; 

    std::vector<Socket> _sockets;
}; 

END_FTC_CORE

#endif