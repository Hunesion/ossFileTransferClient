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

#ifndef _FTC_SYS_UTILS_H_
#define _FTC_SYS_UTILS_H_

#include <string>

std::string getIp();
std::string getIpAndMac();
int getPID();
unsigned long int getThreadId(); 


void ftc_system_init(); 
bool ftc_is_main_thread(); 
int  ftc_run_xdg_open(const std::string &param);


#endif /* _FTC_SYS_UTILS_H_ */