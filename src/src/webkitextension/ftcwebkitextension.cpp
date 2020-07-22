#include <webkit2/webkit-web-extension.h>
#include <JavaScriptCore/JavaScript.h>
#include "ftcwebkitextension.h"
#include "../dbus/ftcappdbus.h"

#include <sys/types.h>
#include <unistd.h>

G_BEGIN_DECLS


static guint32 s_web_view_seq = 0 ; 

static void invokeClientActionCallback(const char* message)
{

    OrgHunesionAppPlayback *proxy = NULL ; 
    GError *error = NULL ; 
#ifdef FTC_DEBUG
    printf("---------------------> invokeClientActionCallback    PID = %d\n", getpid());
#endif
    proxy = org_hunesion_app_playback_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE, FTC_APP_BUS_NAME, FTC_APP_BUS_OBJ_NAME, NULL , &error); 

    if (!proxy) return ; 

    org_hunesion_app_playback_call_invoke_client_action_callback_sync(proxy , s_web_view_seq, message, NULL, &error); 

    if (proxy){
        g_object_unref(proxy); 
    }

    if (error) {
        g_error_free(error);
        error = NULL;
    }
}

static void window_object_cleared_callback(WebKitScriptWorld *world, WebKitWebPage *web_page, WebKitFrame *frame , gpointer user_data){ 
    JSCContext *context = webkit_frame_get_js_context_for_script_world(frame, world); 
    JSCValue *function = jsc_value_new_function(context, "invokeClientAction", G_CALLBACK(invokeClientActionCallback), NULL, NULL, G_TYPE_NONE, 1, G_TYPE_STRING); 

    jsc_context_set_value(context , "invokeClientAction", function) ;
#ifdef FTC_DEBUG
    g_print("-------------------> window_object_cleared_callback\n");
#endif 
}

static gboolean web_page_send_request(WebKitWebPage *web_page, WebKitURIRequest *request , WebKitURIResponse *redirect_response, gpointer user_data){
    const char *request_uri = NULL; 
    const char *page_uri = NULL ;

    request_uri = webkit_uri_request_get_uri(request); 
    page_uri = webkit_web_page_get_uri(web_page); 
#ifdef FTC_DEBUG
    g_print("------------------->Send Request  RequestURI = %s  PageURI = %s\n", request_uri, page_uri); 
#endif
    return FALSE ; 
}

static void 
web_page_created_callback(WebKitWebExtension *extension, WebKitWebPage *web_page, gpointer user_data){
#ifdef FTC_DEBUG
    g_print("------------------->Page %ld created for %s\n", webkit_web_page_get_id(web_page), webkit_web_page_get_uri(web_page)); 
#endif
}


G_MODULE_EXPORT void 
webkit_web_extension_initialize_with_user_data(WebKitWebExtension *extension, GVariant *user_data){
#ifdef FTC_DEBUG
    g_print("-------------------> webkit_web_extension_initialize\n"); 
#endif
    if (user_data && g_variant_is_of_type(user_data, G_VARIANT_TYPE_UINT32)){
        s_web_view_seq = g_variant_get_uint32(user_data); 
    }
    g_signal_connect(extension, "page-created", G_CALLBACK(web_page_created_callback), NULL); 
    g_signal_connect(webkit_script_world_get_default(), "window-object-cleared", G_CALLBACK(window_object_cleared_callback), NULL) ; 
}

G_END_DECLS