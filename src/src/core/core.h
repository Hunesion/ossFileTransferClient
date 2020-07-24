#ifndef _FTC_CORE_CORE_H_
#define _FTC_CORE_CORE_H_


#include "model/GlobalVar.h"
#include "Event.h"
#include "EventManager.h"
#include "Response.h"
#include "JsonUtils.h"
#include "FileInfo.h"
#include "http.h"
#include "sys_utils.h"
#include "urlencode.h"
#include "Logger.h"
#include "SocketServer.h"

bool ftc_core_init(); 
void ftc_core_dispose(); 


#endif /* _FTC_CORE_CORE_H_ */