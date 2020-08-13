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

#ifndef _FTC_CORE_CORE_H_
#define _FTC_CORE_CORE_H_


#include "model/User.h"
#include "model/Properties.h"
#include "model/GlobalVar.h"
#include "model/GlobalStruct.h"
#include "transfer/UploadFile.h"
#include "transfer/Upload.h"
#include "transfer/Download.h"
#include "transfer/DownloadRequest.h"
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
#include "Request.h"
#include "macro.h"

bool ftc_core_init(); 
void ftc_core_dispose(); 


#endif /* _FTC_CORE_CORE_H_ */