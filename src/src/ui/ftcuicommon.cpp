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


#include "ftcuicommon.h"
#include <sstream>


bool        
ftc_ui_is_internal_location()
{
    bool rv = false;

    if (Ftc::Core::GlobalVar::getLoginLocation().getLocation() == "1") {
        rv = true;
    } 

    return rv;
}

GtkResponseType        
ftc_ui_message_box(GtkMessageType type, GtkButtonsType buttons, const char *message)
{
    GtkWidget *msg_dlg = gtk_message_dialog_new(GTK_WINDOW(ftc_ui_get_main_window()), GTK_DIALOG_MODAL, type, buttons, message, NULL);
    GtkResponseType rv = (GtkResponseType)gtk_dialog_run(GTK_DIALOG(msg_dlg));
    if (msg_dlg) {
        gtk_widget_destroy(msg_dlg);
        msg_dlg = NULL;
    }
    return rv;
}

void
ftc_ui_fail_show_message_and_quit(const char *message)
{
  Ftc::Core::EventManager *event_mgr = NULL;

  if (! message) {
    return;
  }

  FTC_LOG("ftc_show_message_and_quit : %s", message);

  ftc_ui_message_box(GtkMessageType::GTK_MESSAGE_ERROR, GtkButtonsType::GTK_BUTTONS_OK, message);

  event_mgr = Ftc::Core::EventManager::getInstance();
  if (! event_mgr) {
    return;
  }
  
  event_mgr->dispatchEvent(FTC_UI_APPLICATION_QUIT, ftc_ui_get_application());
}


bool 
ftc_ui_is_use_clipboard() {
    const Ftc::Core::Properties &properties = Ftc::Core::GlobalVar::getProperties();
    return properties.getUseClipboard();
}

bool 
ftc_ui_is_use_url_redirection() {
    const Ftc::Core::Properties &properties = Ftc::Core::GlobalVar::getProperties();
    return properties.getUseUrlRedirection();
}


void        
ftc_ui_get_widget_text_pixel_size (GtkWidget *widget , const char *string , int * wdret , int * htret )
{
    PangoFontDescription *font_desc = NULL;
    GtkStyleContext *style_context = NULL;
    PangoContext *context = NULL;
    PangoLayout *layout = NULL;

    if (! widget || ! string || ! wdret || ! htret) {
        return;
    }

    style_context = gtk_widget_get_style_context(widget);
    gtk_style_context_save(style_context);
    gtk_style_context_set_state(style_context, GTK_STATE_FLAG_NORMAL);
    gtk_style_context_get(style_context, gtk_style_context_get_state(style_context), "font", &font_desc, NULL);

    context = gtk_widget_get_pango_context ( GTK_WIDGET(ftc_ui_get_main_window()) ) ;
    
    layout = pango_layout_new ( context );
    pango_layout_set_text ( layout, string, -1 );
    pango_layout_set_font_description ( layout, font_desc );
    pango_layout_get_pixel_size (layout, wdret , htret );
    g_object_unref ( layout );
}

std::string 
ftc_ui_reduce_text_pixel(GtkWidget *widget, const char *string, int limit_width)
{
    std::string str;
    std::stringstream rv;
    int wd = 0, ht = 0, dot_wd = 0;

    if (! widget || ! string) {
        return "";
    }

    str = string;

    ftc_ui_get_widget_text_pixel_size(widget, str.c_str(), &wd, &ht);
    if (limit_width >= wd) {
        rv << string;
        return rv.str();
    }

    ftc_ui_get_widget_text_pixel_size(widget, "...", &wd, &ht);
    dot_wd = wd;

    do {
        wd = 0, ht = 0;

        str = str.substr(0, str.length() - 1);
        ftc_ui_get_widget_text_pixel_size(widget, str.c_str(), &wd, &ht);

        if (limit_width > wd + dot_wd) {
            rv << str;
            rv << "...";
            break;
        }
    } while (true);

    return rv.str();
}


std::string 
ftc_ui_get_file_size_format(unsigned int size)
{
    char d2s[10] = { 0, };
    std::string rv = "";
    double bytes = 0;

    if (size >= 0) {
        if (size >= 1073741824) {
            bytes = (double)size / (double)1073741824;
            sprintf(d2s, "%.2lf", bytes);
            rv = d2s;
            rv += " GB";
        } else if (size >= 1048576) {
            bytes = (double)size / (double)1048576;
            sprintf(d2s, "%.2lf", bytes);
            rv = d2s;
            rv += " MB";
        } else if (size >= 1024) {
            bytes = (double)size / (double)1024;
            sprintf(d2s, "%.2lf", bytes);
            rv = d2s;
            rv += " KB";
        } else if (size >= 0 && size < 1024) {
            rv = std::to_string(size) + " Bytes";
        }
    }

    return rv;
}

bool 
ftc_ui_is_expired_date_request_info(const std::string &expired_date/*YYYYMMDD*/)
{
    std::time_t now_date = std::time(NULL), tmp_date, calc_date;
    std::tm now_tm, tmp_tm;
    int seconds = 0;
    bool rv = false;

    localtime_r(&now_date, &now_tm);

    tmp_tm.tm_year = atoi(expired_date.substr(0, 4).c_str()) - 1900;
    tmp_tm.tm_mon = atoi(expired_date.substr(4, 2).c_str()) - 1;
    tmp_tm.tm_mday = atoi(expired_date.substr(6, 2).c_str());
    //  시간 계산 시 초단위 까지 계산함으로 유효 날짜(일 단위)로 표기하기 위해 현재 시분초를 넣어준다.
    tmp_tm.tm_hour = now_tm.tm_hour;
    tmp_tm.tm_min = now_tm.tm_min;
    tmp_tm.tm_sec = now_tm.tm_sec;

    tmp_date = std::mktime(&tmp_tm);
    seconds = std::difftime(tmp_date, now_date);
    if (seconds <= 0) {
        rv = true;
    }

    return rv;
}

GdkPixbuf*  ftc_ui_get_icon_pixbuf(const char *filename, int size) {
    GdkPixbuf *rv = NULL;
    GIcon *gicon = NULL;
    GtkIconTheme *icon_theme = NULL;
    GtkIconInfo *icon_info = NULL;
    const char *mime_type = NULL;

    //  아이콘 이미지
    mime_type = g_content_type_guess(filename, NULL, 0, FALSE);
    if (! mime_type) {
        goto END;
    }
    gicon = g_content_type_get_icon(mime_type);
    if (! gicon) {
        goto END;
    }
    
    icon_theme = gtk_icon_theme_get_default();
    if (! icon_theme) {
        goto END;
    }
    icon_info = gtk_icon_theme_lookup_by_gicon(icon_theme, gicon, size, GtkIconLookupFlags::GTK_ICON_LOOKUP_FORCE_SIZE);
    if (! icon_info) {
        goto END;
    }

    rv = gtk_icon_info_load_icon(icon_info, NULL);
    
END:
    if (mime_type) {
        g_free((gpointer)mime_type);
        mime_type = NULL;
    }

    if (gicon) {
        g_object_unref(gicon);
        gicon = NULL;
    }

    if (icon_info) {
        g_object_unref(icon_info);
        icon_info = NULL;
    }

    return rv;
}


std::string
ftc_ui_get_approval_state_text(const std::string &approval_state, const std::string &approval_type, const std::string &last_approval_type)
{

}