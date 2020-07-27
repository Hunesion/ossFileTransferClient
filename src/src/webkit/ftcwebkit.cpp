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

#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>

#include "ftcwebkit.h"
#include "ftcdbusserver.h"
#include "../core/StringUtils.h"
#include "../core/urlencode.h"


const char * const HTML_PLAIN_TEXT_PRE =  "<html> \n \
<head>  \n \
  <script type='text/javascript'> \n " ; 

const char * const HTML_PLAIN_TEXT_POST = " \n \
    window.addEventListener('load', function(event) { \n \
        submitTarget() ; \n \
    }); \n \
    function submitTarget() { \n \
         var form = document.createElement('form'); \n \
         form.setAttribute('charset', 'UTF-8'); \n \
         form.setAttribute('method', 'Post');  \n \
         form.setAttribute('action', action); \n \
         if (param !== 'undefined' && typeof param === 'string' && param !== ''){ \n \
             try{ \n \
                var json = JSON.parse(param); \n \
                for (var key in json){ \n \
                    var hiddenField = document.createElement('input'); \n \
                    hiddenField.setAttribute('type', 'hidden'); \n \
                    hiddenField.setAttribute('name', key); \n \
                    hiddenField.setAttribute('value', json[key]); \n \
                    form.appendChild(hiddenField); \n \
                } \n \
             }catch (ex){ \n \
             } \n \
         } \
         document.body.appendChild(form); \n \
         form.submit();  \n \
      } \n \
   </script> \n \
</head> \n \
<body> \n \
</body> \n \
</html>\n " ; 


static bool s_web_extension_init = false ; 
static int s_web_view_seq = 0 ; 
static GList *s_dialog_list = NULL ; 
static std::multimap<std::string, std::string> s_javascript_run_started;
static std::multimap<std::string, std::string> s_javascript_run_finished;

/* Signal */
static GtkWidget* ftc_web_view_create_callback(WebKitWebView *web_view, WebKitNavigationAction *navigation_action, gpointer user_data);
static void ftc_web_view_load_changed_callback(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data);
static gboolean ftc_web_view_script_dialog_callback(WebKitWebView *web_view, WebKitScriptDialog *dialog, gpointer user_data);

typedef struct _ftc_web_dialog{
    int seq ; 
    gchar *evt_name ; 
    GtkDialog *dialog ; 
    WebKitWebView *webview ; 
    gchar *json_resp ; 
} FtcWebDialog ; 

static void ftc_web_dialog_free(gpointer data){
    FtcWebDialog *dialog = (FtcWebDialog *)data ; 

    if (!dialog) return ; 
    
    if (dialog->webview){
        g_object_unref(dialog->webview); 
    }
    if (dialog->dialog){
        g_object_unref(dialog->dialog);
    }
    if (dialog->evt_name){
        g_free(dialog->evt_name); 
    }
    if (dialog->json_resp){
        g_free(dialog->json_resp); 
    }

    g_free(dialog); 
}

static bool ftc_web_dialog_add(int seq, const char *event_name, GtkDialog *dialog, WebKitWebView *webview){
    bool rv = false ; 
    FtcWebDialog *item = NULL ; 

    if (dialog == NULL || webview == NULL) return rv ; 

    item = g_new0(FtcWebDialog, 1); 
    item->json_resp = NULL ; 
    item->dialog = dialog ; 
    item->webview = webview ; 
    item->seq = seq ; 
    if (event_name){
        item->evt_name = g_strdup(event_name); 
    }else {
        item->evt_name = NULL ; 
    }

    g_object_ref(item->dialog); 
    g_object_ref(item->webview); 

    s_dialog_list = g_list_append(s_dialog_list, item); 
    rv = true ; 
    return rv ; 
}

static bool ftc_web_dialog_remove_from_seq(int seq){
    bool rv = false ; 
    GList *tmp = NULL ; 
    GList *delItem = NULL ; 

    tmp = s_dialog_list ; 
    while(tmp){
        if (((FtcWebDialog *)(tmp->data))->seq == seq){
            delItem = tmp ; 
            break ;  
        }
        tmp = tmp->next ;
    }

    if (delItem != NULL){
        FtcWebDialog *item =(FtcWebDialog *)delItem->data ; 
        s_dialog_list = g_list_remove(s_dialog_list, item); 
        if (item){
           ftc_web_dialog_free(item) ; 
        }
        rv = true ; 
    }
    return rv ; 
} 


static FtcWebDialog * ftc_web_dialog_get_from_seq(int seq){
    FtcWebDialog *rv = NULL ; 
    GList *tmp = NULL ; 
    tmp = s_dialog_list ; 
    while(tmp){
        if (((FtcWebDialog *)(tmp->data))->seq == seq){
            rv = (FtcWebDialog *)(tmp->data); 
            break ;  
        }
        tmp = tmp->next ;
    }
    return rv ; 
}

static FtcWebDialog *ftc_web_dialog_get_from_dialog(GtkDialog *dialog){
    FtcWebDialog *rv = NULL ; 
    GList *tmp = NULL ; 
    if (!dialog) return rv ; 
    tmp = s_dialog_list ; 

    while(tmp){
        if (((FtcWebDialog *)(tmp->data))->dialog == dialog){
            rv = (FtcWebDialog *)(tmp->data); 
            break ;  
        }
        tmp = tmp->next ; 
    }
    return rv ; 
}

static bool ftc_web_dialog_remove_from_dialog(GtkDialog *dialog){
    bool rv = false ; 
    GList *tmp = NULL ; 
    GList *delItem = NULL ; 

    tmp = s_dialog_list ; 
    while(tmp){
        if (((FtcWebDialog *)(tmp->data))->dialog == dialog){
            delItem = tmp ; 
            break ;  
        }
        tmp = tmp->next ;
    }

    if (delItem != NULL){
        FtcWebDialog *item =(FtcWebDialog *)delItem->data ; 
        s_dialog_list = g_list_remove(s_dialog_list, item); 
        if (item){
           ftc_web_dialog_free(item) ; 
        }
        rv = true ; 
    }
    return rv ; 
}


bool ftc_web_dialog_remove_all(){
    bool rv = false ; 
    g_list_free_full(s_dialog_list, ftc_web_dialog_free); 
    s_dialog_list = NULL ; 
    rv = true ;
    return rv ; 
}



/**
 * Signal 
 */

static void ftc_web_view_close_callback(WebKitWebView *web_view, gpointer user_data){
    if (user_data && GTK_IS_DIALOG(user_data)){
        gtk_dialog_response(GTK_DIALOG(user_data), GtkResponseType::GTK_RESPONSE_CLOSE); 
    }
}

static void ftc_web_view_popup_close_callback(WebKitWebView *web_view, gpointer user_data){
    if (user_data && GTK_IS_DIALOG(user_data)){
        gtk_widget_destroy(GTK_WIDGET(user_data));
    }
}


static void ftc_web_view_dialog_response_callback(GtkDialog *dialog, gint response_id, gpointer user_data){
    FtcWebDialog *web_dialog = NULL ; 
    WebViewResultParam param;
    JsonParser *parser = NULL;
    JsonNode *node = NULL;
    GError *error = NULL;
    std::string evt_name , json_resp; 
    if (!dialog) return ; 

    web_dialog = ftc_web_dialog_get_from_dialog(dialog); 
    if (web_dialog){
        if (web_dialog->evt_name){
            evt_name = web_dialog->evt_name ; 
        }

        if (web_dialog->json_resp){
            json_resp = web_dialog->json_resp; 
        }
    }
    ftc_web_dialog_remove_from_dialog(dialog); 
    gtk_widget_destroy(GTK_WIDGET(dialog)); 

    if (!evt_name.empty()){
        parser = json_parser_new();
        if (json_parser_load_from_data(parser, json_resp.c_str(), json_resp.length(), &error)) {
            node = json_parser_get_root(parser); 
            if (node) {
                param.node = node;
            }
            param.user_data = user_data;
            
            EventManager::getInstance()->dispatchEvent(evt_name.c_str(), (void*)(&param)); 
        }
    }

    if (parser) {
        g_object_unref(parser);
        parser = NULL;
    }

    if (error) {
        g_error_free(error);
        error = NULL;
    }
}

static void web_context_set_web_extensions_directory(WebKitWebContext *context){
    Ftc::Core::FileInfo fileinfo;
    char path[PATH_MAX] = { 0, }; 
    char libdir[PATH_MAX] = { 0, }; 
    FTC_LOG_DEBUG("ftc_web_extension_init"); 
    
    readlink("/proc/self/exe", path, 256);
    fileinfo = Ftc::Core::FileInfo::getFileInfo(path);

    if (fileinfo.isExist()) {
        snprintf(libdir, PATH_MAX, "%s/../lib/ftc", fileinfo.getPath().c_str()); 
        webkit_web_context_set_web_extensions_directory(context, libdir); 
    }
}

/**
 * Init
 */
static void ftc_web_extension_init_callback(WebKitWebContext *context, gpointer user_data){
    web_context_set_web_extensions_directory(context);
}

static bool ftc_web_extension_init(WebKitWebContext *context){
    bool rv = s_web_extension_init; 
    if (rv) return rv ;
    if (!context) return rv ; 
    g_signal_connect(context, "initialize-web-extensions", G_CALLBACK(ftc_web_extension_init_callback), NULL); 
    s_web_extension_init = true ; 
    rv = s_web_extension_init; 
    return rv ; 
}

static WebKitWebContext *ftc_web_get_context(int key){
    WebKitWebContext *rv = NULL ; 
    rv = webkit_web_context_get_default(); 
    web_context_set_web_extensions_directory(rv); 
    webkit_web_context_set_web_extensions_initialization_user_data(rv, g_variant_new_uint32(key)); 
    return rv ; 
}


static bool ftc_web_kit_add_cookie(WebKitWebContext *context){
    bool rv = false ;
    WebKitCookieManager *cookie_manager = NULL ; 
    GSList *list = NULL, *cookie = NULL ; 

    if (!context) return rv ; 

    list = http_get_cookie_list(); 
    if (!list) return rv ; 

    cookie_manager = webkit_web_context_get_cookie_manager(context); 

    if (!cookie_manager) return rv ; 

    for (cookie = list ; cookie ; cookie = cookie->next){
        if (cookie->data){
            webkit_cookie_manager_add_cookie(cookie_manager, soup_cookie_copy((SoupCookie *)(cookie->data)), NULL, NULL, NULL); 
            soup_cookie_free((SoupCookie *)(cookie->data)); 
        }
    }

    g_slist_free(list); 
    rv = true ; 
    return rv ; 
}


static GtkWidget *ftc_web_kit_create_dialog(GtkWindow *parent, const char *title , int width, int height, bool modal, bool debug){
    GtkWidget *rv = NULL , *dialog = NULL  ; 
    dialog = (GtkWidget *)g_object_new (GTK_TYPE_DIALOG,
                         "use-header-bar", true,
                         NULL);
    
    if (title){
        gtk_window_set_title(GTK_WINDOW(dialog), title); 
    }

    gtk_window_set_transient_for (GTK_WINDOW (dialog), parent); 
    if (!debug && modal){
        gtk_window_set_modal (GTK_WINDOW (dialog), TRUE); 
    }
    gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);
    gtk_window_set_default_size (GTK_WINDOW (dialog), width, height);

    rv = dialog ; 
    return rv ; 
}


static WebKitWebView *ftc_web_kit_create_web_view(GtkWidget *parent , WebKitWebView *webView, int key, bool popup, bool debug){
    WebKitWebView *rv = NULL ;
    WebKitWebContext *context = NULL ; 
    WebKitSettings *settings = NULL ;

    if (webView != NULL){ 
        rv =  WEBKIT_WEB_VIEW(webkit_web_view_new_with_related_view(webView)); 
        webkit_web_view_set_settings(rv, webkit_web_view_get_settings(webView)); 
    }else {
        context = ftc_web_get_context(key); 
        if (!context) return rv ; 
        webkit_web_context_set_tls_errors_policy(context, WEBKIT_TLS_ERRORS_POLICY_IGNORE);

        rv = WEBKIT_WEB_VIEW( webkit_web_view_new_with_context(context)); 
        if (!rv) return rv ; 

        settings = webkit_web_view_get_settings(rv); 

        if (debug){
            g_object_set(G_OBJECT(settings), "enable-developer-extras", TRUE, NULL);
        }

        webkit_settings_set_javascript_can_open_windows_automatically(settings, TRUE);
        webkit_settings_set_enable_javascript(settings, TRUE);
        
        ftc_web_kit_add_cookie(context); 
    }
    
    gtk_widget_set_vexpand(GTK_WIDGET(rv), true); 
    gtk_widget_set_hexpand(GTK_WIDGET(rv), true); 

    //signal 
    if (parent) {
        if (popup) {
            g_signal_connect(rv, "close", G_CALLBACK(ftc_web_view_popup_close_callback), parent); 
        } else {
            g_signal_connect(rv, "close", G_CALLBACK(ftc_web_view_close_callback), parent); 
            g_signal_connect(rv, "load-changed", G_CALLBACK(ftc_web_view_load_changed_callback), parent);
        }

        g_signal_connect(rv, "create", G_CALLBACK(ftc_web_view_create_callback), parent);
        g_signal_connect(rv, "script-dialog", G_CALLBACK(ftc_web_view_script_dialog_callback), parent);
    }
    
    return rv ; 
}

static int ftc_web_kit_get_seq(){
    return  ++s_web_view_seq; 
}

void ftc_web_kit_javascript_run_callback(GObject *object, GAsyncResult *res, gpointer user_data) {
    WebKitJavascriptResult *js_result = NULL;
    JSCValue *value = NULL;
    GError *error = NULL;

    js_result = webkit_web_view_run_javascript_finish(WEBKIT_WEB_VIEW(object), res, &error);
    if (! js_result) {
        //  fail
        FTC_LOG("Javascript Failed !! : %s \n", error->message);
    }

    if (error) {
        g_error_free(error);
        error = NULL;
    }
}

void ftc_web_kit_add_javascript_action(const char *url, const char *script, JavascriptRunPoint javascript_run_point) {
    if (javascript_run_point == JavascriptRunPoint::Started) {
        s_javascript_run_started.insert(std::pair<std::string, std::string>(url, script));
    } else if (javascript_run_point == JavascriptRunPoint::Finished) {
        s_javascript_run_finished.insert(std::pair<std::string, std::string>(url, script));
    }
}

bool ftc_web_kit_show_modal_dialog(GtkWindow *parent , const char *url, HTTP_PARAM *param, const char *title , int width, int height, const char *event_name, gpointer user_data, bool modal, bool hide, bool debug){
    bool rv = false ; 
    GtkWidget *dialog = NULL , *content = NULL ; 
    WebKitWebView *webview = NULL ; 
    WebKitWebInspector *inspector = NULL ; 
    int seq = ftc_web_kit_get_seq(); 
    std::string ftc_url = Ftc::Core::GlobalVar::getFtcUrl();
    bool first = true;

#ifndef FTC_DEBUG
    debug = false ; 
#endif

    if (parent == NULL || url == NULL || title == NULL) return rv ; 

    ftc_url.append(url); 
    if (param) {
        ftc_url.append("?");
        for (auto it : *param) {
            if (first) {
                first = false;
            } else {
                ftc_url.append("&");       
            }
            ftc_url.append(it.first);
            ftc_url.append("=");   
            ftc_url.append(url_encode(it.second));
        }
    }
    dialog = ftc_web_kit_create_dialog(parent, title, width, height, modal, debug); 
    if (!dialog) return rv ; 

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog)); 
    if (!content){
        gtk_widget_destroy(GTK_WIDGET(dialog)); 
        return rv ; 
    }

    webview = ftc_web_kit_create_web_view(dialog, NULL,  seq, false, debug); 
    if (!webview){
        gtk_widget_destroy(GTK_WIDGET(dialog)); 
        return rv ; 
    } 

    gtk_container_add(GTK_CONTAINER(content), GTK_WIDGET(webview)); 
    webkit_web_view_load_uri(webview, ftc_url.c_str()); 

    if (debug) {
        inspector = webkit_web_view_get_inspector(webview); 
        if (inspector){
            webkit_web_inspector_show(inspector); 
        }
    }

    FTC_LOG_DEBUG("----->Main PID = %d", getpid()); 

    ftc_web_dialog_add(seq, event_name,  GTK_DIALOG(dialog), webview);
    g_signal_connect(dialog, "response", G_CALLBACK(ftc_web_view_dialog_response_callback), user_data);  
    gtk_widget_show_all(GTK_WIDGET(dialog)); 
    if (hide) {
        gtk_widget_hide(GTK_WIDGET(dialog));    
    } 
    if (!debug && modal) {
        gtk_dialog_run(GTK_DIALOG(dialog)); 
    }
    
    rv = true ; 
    return rv ; 
}

static std::string ftc_web_kit_param_json(const HTTP_PARAM *param){
    std::stringstream ss ; 
    ss << "{"; 
    if (param != NULL && param->size() > 0){
        for (auto it = param->begin() ; it != param->end() ; ++it){
            
            if (it != param->begin()){
                ss << "," ; 
            }

            ss << "\""; 
            ss << it->first; 
            ss << "\""; 
            ss << " : "; 
            ss << "\""; 
            ss << url_encode(it->second); 
            ss << "\""; 
        }
    }
    ss << "}"; 
    return ss.str() ; 
}

bool ftc_web_kit_show_dialog_post(GtkWindow *parent, const char *url, HTTP_PARAM *param, const char *title, int width, int height, const char *event_name , gpointer user_data , bool modal, bool hide, bool debug ){
   bool rv = false ; 
    GtkWidget *dialog = NULL , *content = NULL ; 
    WebKitWebView *webview = NULL ; 
    WebKitWebInspector *inspector = NULL ; 
    int seq = ftc_web_kit_get_seq(); 
    const char *ftc_url = Ftc::Core::GlobalVar::getFtcUrl(); 
    std::string ftc_full_url;

#ifndef FTC_DEBUG
    debug = false ; 
#endif

    if (parent == NULL || url == NULL || title == NULL) return rv ; 

    dialog = ftc_web_kit_create_dialog(parent, title, width, height, modal, debug); 
    if (!dialog) return rv ; 

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog)); 
    if (!content){
        gtk_widget_destroy(GTK_WIDGET(dialog)); 
        return rv ; 
    }

    webview = ftc_web_kit_create_web_view(dialog, NULL,  seq, false, debug); 
    if (!webview){
        gtk_widget_destroy(GTK_WIDGET(dialog)); 
        return rv ; 
    } 

    gtk_container_add(GTK_CONTAINER(content), GTK_WIDGET(webview)); 
    
    std::string html  = ""; 
    html.append(HTML_PLAIN_TEXT_PRE) ; 
    html.append("var action =\""); 
    html.append(url); 
    html.append("\"; "); 
    html.append("var param ='"); 
    if (param != nullptr && param->size() > 0){
        std::string json = ftc_web_kit_param_json(param);
        html.append(json); 
    }else {
        html.append("{}"); 
    }
    html.append("';"); 
    html.append(HTML_PLAIN_TEXT_POST); 

    ftc_full_url.append(ftc_url);
    ftc_full_url.append(url);

    
    webkit_web_view_load_html(webview, html.c_str(), ftc_url); 

    if (debug) {
        inspector = webkit_web_view_get_inspector(webview); 
        if (inspector){
            webkit_web_inspector_show(inspector); 
        }
    }

    FTC_LOG_DEBUG("----->Main PID = %d", getpid()); 

    ftc_web_dialog_add(seq, event_name,  GTK_DIALOG(dialog), webview);
    g_signal_connect(dialog, "response", G_CALLBACK(ftc_web_view_dialog_response_callback), user_data);  
    gtk_widget_show_all(GTK_WIDGET(dialog)); 
    if (hide) {
        gtk_widget_hide(GTK_WIDGET(dialog));    
    } 
    if (!debug && modal) {
        gtk_dialog_run(GTK_DIALOG(dialog)); 
    }
    
    rv = true ; 
    return rv ; 
}

bool ftc_web_kit_dialog_hide(int seq, const gchar *json){
    bool rv = false ;
    FtcWebDialog* web_dialog = NULL ; 

    web_dialog = ftc_web_dialog_get_from_seq(seq); 
    if (!web_dialog) return rv ; 

    if (json){
        web_dialog->json_resp = g_strdup(json); 
    }

    if (web_dialog->dialog){
        gtk_dialog_response(web_dialog->dialog, GtkResponseType::GTK_RESPONSE_OK); 
    }else if (web_dialog->evt_name){
        EventManager::getInstance()->dispatchEvent(web_dialog->evt_name, (void*)(web_dialog->json_resp)); 
        ftc_web_dialog_remove_from_seq(seq); 
    }

    rv = true ;  
    return rv ; 
}



static GtkWidget* ftc_web_view_create_callback(WebKitWebView *web_view, WebKitNavigationAction *navigation_action, gpointer user_data) {    
    FTC_LOG_DEBUG("ftc_web_view_create_callback" );
    GtkWidget *rv = NULL, *dialog = NULL, *content = NULL;
    GtkWindow *parent_window = NULL;
    WebKitURIRequest *request = NULL;
    WebKitWebInspector *inspector = NULL ; 
    int seq = ftc_web_kit_get_seq();

    if (!user_data) {
        return rv;
    }
    parent_window = GTK_WINDOW(user_data);

#ifdef FTC_DEBUG
    dialog = ftc_web_kit_create_dialog(parent_window, "FTC", 400, 400, false, false);
#else
    dialog = ftc_web_kit_create_dialog(parent_window, "FTC", 400, 400, true, false);
#endif

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    rv = GTK_WIDGET(ftc_web_kit_create_web_view(dialog, web_view, seq, true, false));

    gtk_container_add(GTK_CONTAINER(content), GTK_WIDGET(rv));

#ifdef FTC_DEBUG
    inspector = webkit_web_view_get_inspector(WEBKIT_WEB_VIEW(rv)); 
    if (inspector){
        webkit_web_inspector_show(inspector); 
    }
#endif

    gtk_widget_show_all(dialog);

    return rv;
}

static void ftc_web_view_load_changed_callback(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data)
{
    FTC_LOG_DEBUG("ftc_web_view_ready_to_show_callback");
    std::string ftc_url = Ftc::Core::GlobalVar::getFtcUrl(), uri;

    if (! web_view) {
        return;
    }

    uri = webkit_web_view_get_uri(web_view);
    Ftc::Core::StringUtils::replace(uri, ftc_url, "");

    switch ( load_event )
    {
    case WEBKIT_LOAD_STARTED:
        {
            if (s_javascript_run_started.size() > 0) {
                auto range = s_javascript_run_started.equal_range(uri);
                auto start = range.first, end = range.second;

                for (auto it = start; it != end; it++) {
                    webkit_web_view_run_javascript(web_view, it->second.c_str(), NULL, ftc_web_kit_javascript_run_callback, NULL);
                    s_javascript_run_started.erase(it);
                }
            }
            
        }
        break;
    case WEBKIT_LOAD_REDIRECTED:
        {

        }
        break;
    case WEBKIT_LOAD_COMMITTED:
        {

        }
        break;
    case WEBKIT_LOAD_FINISHED:
        {
            if (s_javascript_run_finished.size() > 0) {
                auto range = s_javascript_run_finished.equal_range(uri);
                auto start = range.first, end = range.second;

                for (auto it = start; it != end; it++) {
                    webkit_web_view_run_javascript(web_view, it->second.c_str(), NULL, ftc_web_kit_javascript_run_callback, NULL);
                    s_javascript_run_finished.erase(it);
                }
            }
        }
        break;
    default:
        break;
    }
}

static
gboolean ftc_web_view_script_dialog_callback(WebKitWebView *web_view, WebKitScriptDialog *dialog, gpointer user_data)
{
    FTC_LOG_DEBUG("ftc_web_view_script_dialog_callback");

    WebKitScriptDialogType dialog_type = webkit_script_dialog_get_dialog_type(dialog);   
}