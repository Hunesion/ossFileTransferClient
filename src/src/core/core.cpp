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


