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
#include "ftcloginpage.h"
#include "ftchomepage.h"

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

  FtcLoginPage  *login_page;
  FtcHomePage   *home_page;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcMainWindow, ftc_main_window, GTK_TYPE_APPLICATION_WINDOW);


////////////////////////////////
static FtcMainWindow *s_win = NULL ; 
static bool           s_is_extension = false;

gpointer ftc_ui_get_main_window()
{
  return s_win;
}


void ftc_ui_set_extension_window(gpointer main_win, bool is_ext)
{
  FtcMainWindow *win = NULL;
  FtcMainWindowPrivate *priv = NULL;
  const char *visible_name = NULL;
  if (! FTC_IS_MAIN_WINDOW(main_win)) {
    FTC_LOG_DEBUG("is not main window! ");
    return;
  }

  win = FTC_MAIN_WINDOW(main_win);
  priv = (FtcMainWindowPrivate*)ftc_main_window_get_instance_private(win);

  visible_name = gtk_stack_get_visible_child_name(priv->stack_main);
  if (! visible_name) {
    return;
  }

  //  현재 창이 로그인 창일 경우 확장명령을 캔슬시킨다.
  if (strcmp(visible_name, FTC_VIEW_WINDOW_PAGE_LOGIN) == 0 && is_ext) {
    return;
  }

  gtk_widget_set_visible(GTK_WIDGET(priv->stack_extension), is_ext);
  if (is_ext) {
    //  Extension 모드 일 시
    gtk_window_resize(GTK_WINDOW(win), FTC_UI_MAIN_WINDOW_EXTENSION_WIDTH, FTC_UI_MAIN_WINDOW_EXTENSION_HEIGHT);
  } else {
    gtk_window_resize(GTK_WINDOW(win), FTC_UI_MAIN_WINDOW_WIDTH, FTC_UI_MAIN_WINDOW_HEIGHT);
  }
  
  s_is_extension = is_ext;
}

bool ftc_ui_is_extension_widnow()
{
  return s_is_extension;
}

//////////  FTC 함수  //////////
static void ftc_main_window_update_window_event (Ftc::Core::Event *evt);

static void ftc_main_window_post_login_process(FtcMainWindow *main_window);


static void 
ftc_main_window_init (FtcMainWindow *win)
{
  FtcMainWindowPrivate *priv = NULL;
  gtk_widget_init_template (GTK_WIDGET (win));
  priv = (FtcMainWindowPrivate*)ftc_main_window_get_instance_private(win);
  Ftc::Core::EventManager *event_mgr = NULL; 

  //  Todo..
  s_win = win ; 

  //  리사이즈 불가
  gtk_window_set_resizable(GTK_WINDOW(win), false);
  
  gtk_window_set_default_size(GTK_WINDOW(win), FTC_UI_MAIN_WINDOW_WIDTH, FTC_UI_MAIN_WINDOW_HEIGHT);
  gtk_widget_set_size_request(GTK_WIDGET(priv->stack_main), FTC_UI_MAIN_WINDOW_WIDTH, FTC_UI_MAIN_WINDOW_HEIGHT);
  gtk_widget_set_size_request(GTK_WIDGET(priv->stack_extension), FTC_UI_MAIN_WINDOW_WIDTH, FTC_UI_MAIN_WINDOW_HEIGHT);
  
  //  background color 를 설정한다.
  gtk_widget_set_name(GTK_WIDGET(win), "default_background_window");

  event_mgr = Ftc::Core::EventManager::getInstance();
  if (! event_mgr) {
    ftc_ui_fail_show_message_and_quit("이벤트 매니저 인스턴스 생성 실패");
  } else {
    event_mgr->addEventListener(win, FTC_CORE_LOGIN_LOCATION_EVENT, [](Ftc::Core::Event *evt){
      Ftc::Core::LoginLocation login_location = Ftc::Core::GlobalVar::getLoginLocation();
      Ftc::Core::ClientConfig client_config = Ftc::Core::GlobalVar::getClientConfig();

      if (login_location.isValid()) {
        std::string title;
        title = login_location.getName() + " 자료교환";
        gtk_window_set_title(GTK_WINDOW(s_win), title.c_str());
      }
    });
  } 

  event_mgr->addEventListener(win, FTC_UI_UPDATE_MAIN_STACK_EVENT, HUNE_CALLBACK_CLS_STATIC_1(ftc_main_window_update_window_event));

  
  priv->login_page = ftc_login_page_new();
  gtk_stack_add_named(GTK_STACK(priv->stack_main), GTK_WIDGET(priv->login_page), FTC_VIEW_WINDOW_PAGE_LOGIN);
  gtk_stack_set_visible_child_name(GTK_STACK(priv->stack_main), FTC_VIEW_WINDOW_PAGE_LOGIN);

  ftc_ui_set_extension_window(win, false);
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

static void
ftc_main_window_update_window_event (Ftc::Core::Event *evt)
{
  UpdatePageEventParam *param = NULL;
  FtcMainWindow *win = NULL;
  FtcMainWindowPrivate *priv = NULL;

  if (! evt->getUserData()) {
    return;
  }

  param = (UpdatePageEventParam*)evt->getUserData();

  if (! FTC_IS_MAIN_WINDOW(param->ftc_object)) {
    return;
  }

  win = FTC_MAIN_WINDOW(param->ftc_object);

  priv = (FtcMainWindowPrivate*)ftc_main_window_get_instance_private(win);
  if (!priv) {
    return;
  }

  if (param->page_name == FTC_VIEW_WINDOW_PAGE_HOME) {
    //  로그인 완료 시에 호출된다.
    ftc_main_window_post_login_process(win);
  }
}


static void
ftc_main_window_post_login_process(FtcMainWindow *main_window)
{
  FtcMainWindowPrivate *priv = NULL;
  if (! FTC_IS_MAIN_WINDOW(main_window)) {
    return;
  }

  priv = (FtcMainWindowPrivate*)ftc_main_window_get_instance_private(main_window);
  if (! priv) {
    return;
  }

  if (! priv->home_page) {
    priv->home_page = ftc_home_page_new();
    gtk_stack_add_named(GTK_STACK(priv->stack_main), GTK_WIDGET(priv->home_page), FTC_VIEW_WINDOW_PAGE_HOME);
  }
  gtk_stack_set_visible_child_name(GTK_STACK(priv->stack_main), FTC_VIEW_WINDOW_PAGE_HOME);

  ftc_login_page_set_login_state(priv->login_page, true);
}
