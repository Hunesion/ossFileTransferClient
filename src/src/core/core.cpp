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

#include <pthread.h>
#include "core.h"

USING_FTC_CORE; 

static bool s_core_init = false ;  

bool ftc_core_init(){
    bool rv = false ;
    EventManager *evtManager = NULL ; 
    SocketServer *sockServer = NULL ; 

    GlobalVar::init(); 
    evtManager = EventManager::getInstance(); 
    sockServer = SocketServer::getInstance(); 
    if (evtManager == nullptr || sockServer == nullptr) return rv;

    ftc_system_init();

    rv = true ; 
    s_core_init = true ; 
    return rv ; 
}


void ftc_core_dispose(){
    if (!s_core_init) return ; 
    EventManager::getInstance()->destroyInstance(); 
    SocketServer::getInstance()->destroyInstance(); 
    
    http_dispose(); 
    GlobalVar::dispose(); 
    s_core_init = false ; 
}


