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


#ifndef _FTC_UI_FTC_UI_COMMON_H_
#define _FTC_UI_FTC_UI_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <gtk/gtk.h>
#include <ctime>
#include "Event.h"
#include "../core/core.h"
#include "../webkit/ftcwebkit.h"

#define FTC_UI_MAIN_WINDOW_WIDTH                420
#define FTC_UI_MAIN_WINDOW_HEIGHT               640
#define FTC_UI_MAIN_WINDOW_EXTENSION_WIDTH      (FTC_UI_MAIN_WINDOW_WIDTH * 2)
#define FTC_UI_MAIN_WINDOW_EXTENSION_HEIGHT     FTC_UI_MAIN_WINDOW_HEIGHT

typedef struct _UpdatePageEventParam
{
    gpointer ftc_object;
    std::string page_name;
    void *user_data1;
    void *user_data2;
    void *user_data3;

    _UpdatePageEventParam()
    {
        ftc_object = NULL;
        user_data1 = NULL;
        user_data2 = NULL;
        user_data3 = NULL;
    }
} UpdatePageEventParam;


#define FTC_VIEW_WINDOW_PAGE_LOGIN          "login_page"
#define FTC_VIEW_WINDOW_PAGE_HOME           "home_page"
#define FTC_VIEW_WINDOW_PAGE_SEND_DETAIL    "send_detail_page"
#define FTC_VIEW_WINDOW_PAGE_SEND_DETAIL_ONLY_EXTENSION "send_detail_page_only_extension"
#define FTC_VIEW_WINDOW_PAGE_RECV_DETAIL    "recv_detail_page"
#define FTC_VIEW_WINDOW_PAGE_RECV           "recv_page"
#define FTC_VIEW_WINDOW_PAGE_SEND_HISTORY_DETAIL   "send_history_detail_page"
#define FTC_VIEW_HOME_HEADER_SEND           "header_send_box"
#define FTC_VIEW_HOME_HEADER_RECV           "header_recv_box"
#define FTC_VIEW_HOME_HEADER_SEND_HISTORY   "header_send_history_box"
#define FTC_VIEW_HOME_SUB_PAGE_DND          "dnd_sub_page"
#define FTC_VIEW_HOME_SUB_PAGE_SEND_LIST    "send_list_sub_page"
#define FTC_VIEW_HOME_SUB_PAGE_RECV_LIST    "recv_list_sub_page"
#define FTC_VIEW_HOME_SUB_PAGE_SEND_HISTORY "send_history_sub_page"
#define FTC_VIEW_HOME_SEND_LIST_BOX_INFO    "send_list_info_box"
#define FTC_VIEW_HOME_SEND_LIST_PROGRESS    "send_list_progress"
#define FTC_VIEW_DETAIL_RECV_SUB_PAGE_FILE_LIST "detail_recv_file_list_sub_page"
#define FTC_VIEW_DETAIL_RECV_SUB_PAGE_REQUEST_INFO "detail_recv_request_info_sub_page"
#define FTC_VIEW_DETAIL_SEND_HISTORY_SUB_PAGE_FILE_LIST "detail_send_history_file_list_sub_page"
#define FTC_VIEW_DETAIL_SEND_HISTORY_SUB_PAGE_REQUEST_INFO "detail_send_history_request_info_sub_page"

gpointer    ftc_ui_get_main_window();

gpointer    ftc_ui_get_application();

bool        ftc_ui_is_internal_location();

GtkResponseType ftc_ui_message_box(GtkMessageType type, GtkButtonsType buttons, const char *message);

#endif /* _FTC_UI_FTC_UI_COMMON_H_ */