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


#ifndef _FTC_UI_APPLICATION_H_
#define _FTC_UI_APPLICATION_H_
#include "ftcuicommon.h"

#define FTC_TYPE_APPLICATION (ftc_application_get_type ())
G_DECLARE_FINAL_TYPE (FtcApplication, ftc_application, FTC, APPLICATION, GtkApplication)


FtcApplication     *ftc_application_new         (void);


#endif /* _FTC_UI_APPLICATION_H_ */
