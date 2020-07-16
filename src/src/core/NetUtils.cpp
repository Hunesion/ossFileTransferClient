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

#include <ifaddrs.h>
#include <netpacket/packet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include "StringUtils.h"
#include "NetUtils.h"

BEGIN_FTC_CORE

namespace NetUtils
{


struct ifaddrs* getIfAddrs(struct ifaddrs* ifaddrs , const char* name, int family){
    struct ifaddrs* rv = NULL ; 
    struct ifaddrs *ifa = NULL;
    if (ifaddrs == NULL || name == NULL) return rv ; 

    for (ifa = ifaddrs; ifa != NULL; ifa = ifa->ifa_next){
        if (ifa->ifa_name == NULL || ifa->ifa_addr->sa_family != family) continue ; 

        if (strcmp(ifa->ifa_name, name) == 0){
            rv = ifa ; 
            break ; 
        }
    }
    return rv ; 
}

void setInetAddr(NetInfo& info , struct ifaddrs* ifa){
    char host[NI_MAXHOST]; 
    if (ifa == NULL) return ; 

    if (getnameinfo(ifa->ifa_addr , sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL , 0 , NI_NUMERICHOST) != 0) return  ;
    info.inetAddr = host ; 
}

bool setNetInfo(std::list<NetInfo> &list, struct ifaddrs *ifaddrs)
{
    bool rv = false;
    struct ifaddrs *ifa = NULL;
    struct ifaddrs *inet = NULL;
    if (ifaddrs == NULL)
        return rv;

    for (ifa = ifaddrs; ifa != NULL; ifa = ifa->ifa_next)
    {   
        NetInfo info ; 
        char temp[256]; 
        if ((ifa->ifa_addr) && (ifa->ifa_addr->sa_family == AF_PACKET))
        {
            struct sockaddr_ll *s = (struct sockaddr_ll *)ifa->ifa_addr;

            if (s == NULL || ifa->ifa_name == NULL || strcmp(ifa->ifa_name, "lo") == 0) continue; 
            info.ifname = ifa->ifa_name ;

            for (int i = 0 ; i < s->sll_halen ; i++){
                snprintf(temp, 256, "%02X", s->sll_addr[i]); 
                info.macAddr.append(temp); 
                if ( i+1 != s->sll_halen) {
                    info.macAddr.append(":");
                }
            }

            inet = getIfAddrs(ifaddrs, ifa->ifa_name , AF_INET); 
            if (inet != NULL){
                setInetAddr(info, inet); 
            }

            list.push_back(info); 
        }
    }
    rv = true ; 
    return rv;
}

std::list<NetInfo> getNetInfo()
{
    std::list<NetInfo> rv;

    struct ifaddrs *ifaddrs = NULL;

    if (getifaddrs(&ifaddrs) < 0)
    {
        goto DONE;
    }

    if (ifaddrs == NULL)
    {
        return rv;
    }

    if (!setNetInfo(rv, ifaddrs))
    {
        goto DONE;
    }

DONE:
    if (ifaddrs != NULL)
    {
        freeifaddrs(ifaddrs);
        ifaddrs = NULL;
    }

    return rv;
}
} // namespace NetUtils

END_FTC_CORE