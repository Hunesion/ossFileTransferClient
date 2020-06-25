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


#ifndef _FTC_UI_MAIN_WINDOW_H_
#define _FTC_UI_MAIN_WINDOW_H_
#include "ftcuicommon.h"
#include "ftcapplication.h"

#define FTC_TYPE_MAIN_WINDOW (ftc_main_window_get_type ())
G_DECLARE_FINAL_TYPE (FtcMainWindow, ftc_main_window, FTC, MAIN_WINDOW, GtkApplicationWindow)



FtcMainWindow       *ftc_main_window_new          (FtcApplication *app);
void                 ftc_main_window_open         (FtcMainWindow *main_window,
                                                         GFile            *file);


#endif /* _FTC_UI_MAIN_WINDOW_H_ */
