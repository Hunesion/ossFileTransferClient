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

    virtual ~SocketServer(); 
private: 
    SocketServer();
    
    bool start(int port);
    bool stop();  

    bool work(std::string data);
    bool clipboard(std::string &data, std::vector<std::string> &splitData);
    bool urlRedirection(std::string &data, std::vector<std::string> &splitData);
    bool autoDownload(std::string &data, std::vector<std::string> &splitData);

    std::list<std::string> downloadFile(std::string requestInfoUid, std::string localIp, const std::string &dir);

private: 
    static SocketServer *s_instance ; 
    static volatile bool s_stop ; 

    std::vector<Socket> _sockets;
}; 

END_FTC_CORE

#endif