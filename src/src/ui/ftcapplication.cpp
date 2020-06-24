#include "ftcapplication.h"
#include "ftcmainwindow.h"

struct _FtcApplication
{
  GtkApplication parent;
};

G_DEFINE_TYPE(FtcApplication, ftc_application, GTK_TYPE_APPLICATION);

static FtcApplication *s_app = NULL ;


static void
ftc_application_init (FtcApplication *app)
{
  
}

static void
ftc_application_activate (GApplication *app)
{
  FtcMainWindow *win;
  GtkCssProvider *provider = NULL;
  GtkBuilder *builder = NULL;
  GMenuModel *app_tray_menu = NULL;

  win = ftc_main_window_new (FTC_APPLICATION (app));
  gtk_window_present (GTK_WINDOW (win));
}

static void
ftc_application_open (GApplication  *app,
                  GFile        **files,
                  gint           n_files,
                  const gchar   *hint)
{
  GList *windows;
  FtcMainWindow *win;
  int i;

  windows = gtk_application_get_windows (GTK_APPLICATION (app));
  if (windows){
    win = FTC_MAIN_WINDOW (windows->data);
  } else {
    win = ftc_main_window_new (FTC_APPLICATION (app));
  }
  
  for (i = 0; i < n_files; i++){
    ftc_main_window_open (win, files[i]);
  }

  gtk_window_present (GTK_WINDOW (win));
}

static void ftc_applcation_shutdown(GApplication *app){
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
