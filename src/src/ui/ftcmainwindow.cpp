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


#include "ftcmainwindow.h"

#include <mutex>
#include <sstream>

//////////  구현부  ////////

struct _FtcMainWindow
{
    GtkApplicationWindow parent;
};

typedef struct _FtcMainWindowPrivate FtcMainWindowPrivate;
struct _FtcMainWindowPrivate
{
  /* template에서 정의 */
  GtkStack      *stack_main;
  GtkStack      *stack_extension;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcMainWindow, ftc_main_window, GTK_TYPE_APPLICATION_WINDOW);


////////////////////////////////

//////////  FTC 함수  //////////



static void 
ftc_main_window_init (FtcMainWindow *win)
{
  FtcMainWindowPrivate *priv = NULL;
  gtk_widget_init_template (GTK_WIDGET (win));
  priv = (FtcMainWindowPrivate*)ftc_main_window_get_instance_private(win);

  //  Todo..

  //  리사이즈 불가
  gtk_window_set_resizable(GTK_WINDOW(win), false);
  
  gtk_window_set_default_size(GTK_WINDOW(win), FTC_UI_MAIN_WINDOW_WIDTH, FTC_UI_MAIN_WINDOW_HEIGHT);
  gtk_widget_set_size_request(GTK_WIDGET(priv->stack_main), FTC_UI_MAIN_WINDOW_WIDTH, FTC_UI_MAIN_WINDOW_HEIGHT);
  gtk_widget_set_size_request(GTK_WIDGET(priv->stack_extension), FTC_UI_MAIN_WINDOW_WIDTH, FTC_UI_MAIN_WINDOW_HEIGHT);
  
 
}

static void
ftc_main_window_dispose (GObject *object)
{
  FtcMainWindowPrivate *priv = NULL;
  priv = (FtcMainWindowPrivate*)ftc_main_window_get_instance_private(FTC_MAIN_WINDOW(object));


  G_OBJECT_CLASS(ftc_main_window_parent_class)->dispose(object);
}

static void
ftc_main_window_finalize(GObject *object)
{
  G_OBJECT_CLASS(ftc_main_window_parent_class)->finalize(object);
}

static void 
ftc_main_window_class_init (FtcMainWindowClass *clz)
{
  G_OBJECT_CLASS (clz)->finalize = ftc_main_window_finalize;
  G_OBJECT_CLASS (clz)->dispose = ftc_main_window_dispose;

  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-main-window.ui");
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcMainWindow, stack_main);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcMainWindow, stack_extension);
}

FtcMainWindow*
ftc_main_window_new (FtcApplication *app)
{
  return (FtcMainWindow *)g_object_new (FTC_TYPE_MAIN_WINDOW, "application", app, NULL);
}

void 
ftc_main_window_open (FtcMainWindow *main_window, GFile *file)
{

}