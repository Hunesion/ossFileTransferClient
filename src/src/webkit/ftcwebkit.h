#ifndef _FTC_WEB_KIT_H_
#define _FTC_WEB_KIT_H_
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "../core/core.h"


enum JavascriptRunPoint
{
    Started = 0,
    Finished
};


typedef struct _WebViewResultParam
{
    gpointer user_data;
    JsonNode *node;

    _WebViewResultParam() {
        user_data = NULL;
        node = NULL;
    }
} WebViewResultParam;

void ftc_web_kit_add_javascript_action(const char *url, const char *script, JavascriptRunPoint javascript_run_point);
bool ftc_web_kit_show_modal_dialog(GtkWindow *parent, const char *url, HTTP_PARAM *param, const char *title, int width, int height, const char *event_name = NULL, gpointer user_data = NULL, bool modal = true, bool hide = false, bool debug = true);
bool ftc_web_kit_show_dialog_post(GtkWindow *parent, const char *url, HTTP_PARAM *param, const char *title, int width, int height, const char *event_name = NULL, gpointer user_data = NULL, bool modal = true, bool hide = false, bool debug = true);
bool ftc_web_kit_dialog_hide(int seq , const gchar *json); 
bool ftc_web_dialog_remove_all();
#endif