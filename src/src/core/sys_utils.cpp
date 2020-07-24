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
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include <fcntl.h>

#include "sys_utils.h"
#include "NetUtils.h"
#include "Logger.h"

static std::string s_ips;
static std::string s_ip_macs;

static pthread_t s_main_thread = 0;

static bool initIpMac()
{
    bool rv = false;
    bool first = true;

    if (!s_ips.empty() && !s_ip_macs.empty())
    {
        rv = true;
        return rv;
    }

    std::list<NetInfo> list = Ftc::Core::NetUtils::getNetInfo();

    if (list.size() == 0)
        return rv;

    for (auto &ele : list)
    {
        if (!first)
        {
            s_ips.append(",");
            s_ip_macs.append(",");
        }
        s_ips.append(ele.inetAddr);
        s_ip_macs.append(ele.inetAddr);
        s_ip_macs.append("_");
        s_ip_macs.append(ele.macAddr);
        first = false ; 
    }

#ifdef FTC_DEBUG
    FTC_LOG_DEBUG("IP ADDRES = %s", s_ips.c_str());
    FTC_LOG_DEBUG("IP MAC ALL = %s", s_ip_macs.c_str());
#endif

    rv = true;
    return rv;
}

std::string getIp()
{
    initIpMac();
    return s_ips;
}
std::string getIpAndMac()
{
    initIpMac();
    return s_ip_macs;
}


int getPID()
{
    return getpid();
}

unsigned long int getThreadId()
{
    return pthread_self();
}

void ftc_system_init()
{
    s_main_thread = pthread_self();
}
bool ftc_is_main_thread()
{
    return s_main_thread == pthread_self();
}

int
ftc_open_uri(const std::string &param)
{
    GError *error  = NULL;

    if (g_app_info_launch_default_for_uri(param.c_str(), NULL, &error)) {
        return 0;
    }

    FTC_LOG("OPEN URI FAIL : %s", param.c_str());
    g_error_free(error);

    return 1;
}
int
ftc_run_xdg_open(const std::string &param)
{
    int rv = 0;
    std::stringstream ss;
    ss << "xdg-open \"";
    ss << param;
    ss << "\"";

    rv = system(ss.str().c_str());
    if (rv != 0) {
        FTC_LOG("XDG OPEN FAIL : %s", param.c_str());
    } 

    return rv;
}
