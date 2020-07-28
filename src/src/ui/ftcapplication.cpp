#include "ftcapplication.h"
#include "ftcmainwindow.h"
#include "FtcEventCallback.h"
#include "../webkit/ftcdbusserver.h"


struct _FtcApplication
{
  GtkApplication parent;
};

G_DEFINE_TYPE(FtcApplication, ftc_application, GTK_TYPE_APPLICATION);

static FtcApplication *s_app = NULL ;


gpointer 
ftc_ui_get_application()
{
  return s_app;
}

static void ftc_application_init_config(FtcApplication *app);

static void ftc_application_quit(FtcApplication *app);

static void ftc_application_quit_event(Ftc::Core::Event *evt);

static void ftc_application_tray_menu_item_quit(GSimpleAction *action, GVariant *parameter, gpointer data);



static GActionEntry app_entries[] =
{
  { "quit", ftc_application_tray_menu_item_quit, NULL, NULL, NULL }
};



static void
ftc_application_init (FtcApplication *app)
{
  Ftc::Core::EventManager::getInstance()->addEventListener(app, FTC_UI_APPLICATION_QUIT, HUNE_CALLBACK_CLS_STATIC_1(ftc_application_quit_event));
}

static void
ftc_application_activate (GApplication *app)
{
  FtcMainWindow *win = NULL;

  ftc_application_init_config(FTC_APPLICATION(app));
  
  win = ftc_main_window_new (FTC_APPLICATION (app));
  gtk_window_present (GTK_WINDOW (win));
}

static void
ftc_application_open (GApplication  *app,
                  GFile        **files,
                  gint           n_files,
                  const gchar   *hint)
{
  GList *windows = NULL;
  FtcMainWindow *win = NULL;
  int i = 0;

  windows = gtk_application_get_windows (GTK_APPLICATION (app));
  if (windows){
    win = FTC_MAIN_WINDOW (windows->data);
  } else {
    ftc_application_init_config(FTC_APPLICATION(app));
    win = ftc_main_window_new (FTC_APPLICATION (app));
  }

  for (i = 0; i < n_files; i++){
    ftc_main_window_open (win, files[i]);
  }

  gtk_window_present (GTK_WINDOW (win));
}

static void ftc_applcation_shutdown(GApplication *app){
  Ftc::Core::EventManager::getInstance()->removeEventListener(app);
  ftc_core_dispose();  
  ftc_event_callback_dispose();
  ftc_dbus_server_destory(); 
  ftc_web_dialog_remove_all(); 
  s_app = NULL;
  G_APPLICATION_CLASS(ftc_application_parent_class)->shutdown(app);
}

static void
ftc_application_class_init (FtcApplicationClass *clz)
{
  G_APPLICATION_CLASS (clz)->activate = ftc_application_activate;
  G_APPLICATION_CLASS (clz)->open = ftc_application_open;
  G_APPLICATION_CLASS (clz)->shutdown = ftc_applcation_shutdown; 
}

FtcApplication *
ftc_application_new (void)
{
  s_app = (FtcApplication *)g_object_new (FTC_TYPE_APPLICATION,
                       "application-id", "com.hunesion.ftcapp",
                       "flags", G_APPLICATION_HANDLES_OPEN,
                       NULL);
  return s_app;
}

static void 
ftc_application_init_config(FtcApplication *app)
{
  GtkCssProvider *provider = NULL;
  GtkBuilder *builder = NULL;
  GMenuModel *app_tray_menu = NULL;
  const gchar *quit_accesls[2] = { "<Ctrl>Q", NULL };
  
  ftc_core_init();
  ftc_event_callback_init();
  ftc_dbus_server_init(); 
  
  g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);

  gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.quit", quit_accesls);

  //  CSS 파일 전역으로 로딩
  //
  provider = gtk_css_provider_new();
  gtk_css_provider_load_from_resource(provider, "/com/hunesion/ftcapp/style.css"); 
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);  

  if (builder) {
    g_object_unref(builder);
    builder = NULL;
  }
  
  if (provider) {
    g_object_unref(provider);
    provider = NULL;
  }
}

static void
ftc_application_quit(FtcApplication *app)
{
  if (! FTC_IS_APPLICATION(app)) {
    return;
  }

  g_application_quit(G_APPLICATION(app));
}

static void
ftc_application_quit_event(Ftc::Core::Event *evt)
{
  FtcApplication *app = NULL;

  if (!evt) {
    return;
  }

  if (!FTC_IS_APPLICATION(evt->getUserData())) {
    return;
  }

  app = FTC_APPLICATION(evt->getUserData());

  ftc_application_quit(app);
}

static void
ftc_application_tray_menu_item_open(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  GtkApplication *app = NULL;
  GList *list = NULL;

  if (! FTC_IS_APPLICATION(data)) {
    return;
  }
  app = GTK_APPLICATION(data);

  list = gtk_application_get_windows (GTK_APPLICATION (app));

  if (list) {
    if (! GTK_IS_WINDOW(list->data)) {
      return;
    }

    gtk_window_present(GTK_WINDOW(list->data));
  }
}

static void 
ftc_application_tray_menu_item_approval_management(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  
}

static void 
ftc_application_tray_menu_item_send_history(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  
}

static void 
ftc_application_tray_menu_item_send_clipboard(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  
}

static void 
ftc_application_tray_menu_item_send_url(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  
}

static void 
ftc_application_tray_menu_item_allow_url(GSimpleAction *action, GVariant *parameter, gpointer data)
{

}

static void
ftc_application_tray_menu_item_quit(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  FtcApplication *app = NULL;

  if (!FTC_IS_APPLICATION(data)) {
    return;
  }

  app = FTC_APPLICATION(data);
  ftc_application_quit(app);
}