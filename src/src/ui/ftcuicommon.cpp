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
