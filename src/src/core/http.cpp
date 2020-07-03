#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>
#include "http.h"
#include "EventManager.h"
#include "Event.h"
#include "FileInfo.h"
#include "StringUtils.h"

#define FTC_URL_DEFINE         ""

#define CONTENT_TYPE "application/x-www-form-urlencoded"
#define MULTI_PART "multipart/form-data"
#define OCTET_STREAM "application/octet-stream"

#define HTTP_CONNECT_TIMEOUT 300 
#define HTTP_RESPONSE_TIMEOUT 300

#define HTTP_RESPONSE_OK 200 
#define ASYNC_READ_BLOCK_SIZE    8192

static std::string s_cookies ; 
static SoupSession *s_session = NULL ; 

USING_FTC_CORE ; 

static SoupSession* http_get_session(); 
static SoupCookieJar *http_get_cookie_jar(); 

void http_add_param(HTTP_PARAM &param , std::string key, std::string value){
    param.insert(std::pair<std::string, std::string>(key, value)); 
}

void http_add_header(HTTP_HEADER &header , std::string key, std::string value){
    header.insert(std::pair<std::string, std::string>(key, value)); 
}

static SoupURI *http_get_uri(){
    SoupURI *rv = NULL ; 
    const char *ftc_url = FTC_URL_DEFINE;
    rv = soup_uri_new(ftc_url); 
    return rv ; 
}

GSList *http_get_cookie_list(){
    GSList *rv = NULL ; 
    SoupCookieJar *cookie_jar = NULL ; 
    SoupURI *uri = NULL ; 
    cookie_jar = http_get_cookie_jar(); 
    if (!cookie_jar) return rv ; 
    uri = http_get_uri(); 
    if (uri){
        rv = soup_cookie_jar_get_cookie_list(cookie_jar, uri, true); 
    }
    if (uri){
        soup_uri_free(uri); 
    }
    return rv ; 
}

void http_clear_cookies(){
    SoupCookieJar *cookie_jar = NULL ; 
    GSList *l = NULL, *e  = NULL ; 
    cookie_jar = http_get_cookie_jar(); 
    if (!cookie_jar) return ;
    
    l = soup_cookie_jar_all_cookies(cookie_jar); 
    if (!l) return ; 

    for (e = l ; e ; e = e->next){
        soup_cookie_jar_delete_cookie(cookie_jar , (SoupCookie*)(e->data)); 
        soup_cookie_free((SoupCookie*)(e->data)); 
    }
    g_slist_free(l); 
}

static int read_response_cookies(SoupMessageHeaders *header){
    int rv = 0 ; 
    SoupURI *uri = NULL ; 
    SoupCookieJar *cookie_jar = NULL ; 
    if (header == NULL){
        return rv ; 
    }

    const char* cookie_str = soup_message_headers_get_one(header, "Set-Cookie"); 

    if (cookie_str != NULL){
        s_cookies = cookie_str ; 
        uri = http_get_uri() ; 
        if (uri != NULL){
            cookie_jar = http_get_cookie_jar(); 
            if (cookie_jar){
                soup_cookie_jar_set_cookie(cookie_jar, uri, cookie_str); 
            }
        }
    }

    if (uri){
        soup_uri_free(uri); 
        uri = NULL ; 
    }

    rv = 1 ; 
    return rv ; 
}

static void http_set_message_header(const char *name,
                                    const char *value,
                                    gpointer    user_data)
{
    Response *resp = (Response*)user_data;

    resp->setHeaderMap(name, value);
}


static SoupSession* http_create_session(){
    SoupSession* rv = NULL ; 
    SoupLogger *logger = NULL;
    SoupCookieJar *cookie_jar = NULL ; 
    rv = soup_session_new_with_options(SOUP_SESSION_SSL_STRICT, 0, 
#ifdef FTC_DEBUG
                SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_CONTENT_SNIFFER, 
#endif
                SOUP_SESSION_MAX_CONNS , 20, 
                SOUP_SESSION_MAX_CONNS_PER_HOST, 10 , 
                SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_COOKIE_JAR,
                SOUP_SESSION_TIMEOUT , HTTP_CONNECT_TIMEOUT , 
                SOUP_SESSION_IDLE_TIMEOUT , HTTP_RESPONSE_TIMEOUT, 
    NULL);
    if (!rv) return rv;
    
#ifdef FTC_DEBUG
    logger = soup_logger_new(SOUP_LOGGER_LOG_BODY, -1);
    if (logger)
    {
        soup_session_add_feature(rv, SOUP_SESSION_FEATURE(logger));
        g_object_unref(logger);
    }
#endif 

    cookie_jar = SOUP_COOKIE_JAR(soup_session_get_feature(rv, SOUP_TYPE_COOKIE_JAR)); 
    if (cookie_jar){
        soup_cookie_jar_set_accept_policy(cookie_jar, SoupCookieJarAcceptPolicy::SOUP_COOKIE_JAR_ACCEPT_ALWAYS); 
    }
    return rv ; 
}

static SoupSession* http_get_session(){
    SoupSession* rv = s_session ; 
    if (rv) return rv ; 
    rv = http_create_session() ; 
    s_session = rv ; 
    return rv ; 
}

static SoupCookieJar *http_get_cookie_jar(){
    SoupCookieJar *jar = NULL ; 
    SoupSession *session = NULL ; 
    session = http_get_session(); 
    if (!session) return jar ; 
    jar = SOUP_COOKIE_JAR(soup_session_get_feature(session, SOUP_TYPE_COOKIE_JAR)); 
    return jar ; 
}


static void http_param_append(std::stringstream &ss , const std::string &key, const std::string &value , bool json){
    if (json){
        ss << "\""; 
        ss << key; 
        ss << "\""; 
        ss << " : "; 
        ss << "\""; 
        ss << value ; 
        ss << "\""; 
    }else {
        ss << key; 
        ss << "="; 
        ss << value ; 
    }
}

static std::string http_create_param(const HTTP_PARAM *param , bool crypto){
    std::stringstream ss ; 
    int first = true , index = 0 ; 
    if (param != NULL && param->size() > 0){
        for (auto it = param->begin() ; it != param->end() ; ++it){    
            if (it != param->begin()){
                ss << "&" ; 
            }           
            http_param_append(ss, it->first , it->second , false); 

            first = false ; 
        }
    }

    return ss.str() ; 
}

static SoupMessage* http_create_message(const char *method , const char *url, const HTTP_PARAM *param , const HTTP_HEADER *header, bool crypto){
    SoupMessage* rv = NULL ; 
    std::string log;
    std::string ftc_url = FTC_URL_DEFINE;

    if (method == NULL || url == NULL || ftc_url.empty()) return rv ; 
    
    ftc_url.append(url);

    if (param != NULL && strncmp("GET", method , 3) == 0){
        std::string paramStr = http_create_param(param, crypto);
        ftc_url.append("?"); 
        ftc_url.append(paramStr);  
    }

    rv = soup_message_new(method , ftc_url.c_str()); 

    if (!rv) return rv ; 

    
    if (strncmp("POST", method, 4) == 0){
        std::string paramStr = http_create_param(param, crypto); 
        if (!paramStr.empty()){
             soup_message_set_request(rv, CONTENT_TYPE, SOUP_MEMORY_COPY, paramStr.c_str(), strlen(paramStr.c_str())); 
        }
    }

    if (header != NULL) {
        for (auto it = header->begin(); it != header->end(); it++) {
            soup_message_headers_append(rv->request_headers, it->first.c_str(), it->second.c_str());
        }
    }

    return rv ; 
}

static std::shared_ptr<Response> http_send_request(const char *method , const char *url , const HTTP_PARAM *param , const HTTP_HEADER *header, bool crypto){
    auto rv = std::make_shared<Response>(url) ; 
    JsonNode *node = NULL;
    SoupSession *session = NULL;
    SoupMessage *msg = NULL;
    GError *error = NULL;
    JsonParser *parser = NULL;
    int status = 0 ; 
    SoupURI *uri = NULL;
    const char *error_msg = NULL;

    if (url == NULL || method == NULL)
        return rv;

    session = http_get_session(); 
    if (!session) return rv ; 
    msg = http_create_message(method, url , param, header, crypto); 

    if (!msg)
        goto END;

    status = soup_session_send_message(session, msg); 
    rv->setStatus(status); 

    soup_message_headers_foreach(msg->response_headers, http_set_message_header, rv.get());
    if (SOUP_STATUS_IS_SUCCESSFUL(status)){
        read_response_cookies(msg->response_headers); 
        parser = json_parser_new(); 
        if (!parser) goto END ; 
        if (json_parser_load_from_data(parser, msg->response_body->data, msg->response_body->length, &error)) {
            node = json_parser_get_root(parser); 
            if (node){
                node = json_node_copy(node); 
            }
        }else {
            rv->setResponse(msg->response_body->data, msg->response_body->length); 
        }
    } else {
        error_msg = soup_status_get_phrase(status);
    }
    
END:

    if (error){
        g_error_free(error); 
        error = NULL ; 
    }

    if (parser)
    {
        g_object_unref(parser);
        parser = NULL;
    }

    if (msg)
    {
        g_object_unref(msg);
        msg = NULL;
    }

    if (node){
        rv->setJsonNode(node); 
        json_node_unref(node); 
    }

    return rv;
}


std::shared_ptr<Response> http_send_get(const char *url , const HTTP_PARAM *param, const HTTP_HEADER *header)
{
    return http_send_request("GET", url , param , header, false); 
}

std::shared_ptr<Response> http_send_post(const char *url, const HTTP_PARAM *param , const HTTP_HEADER *header, bool crypto){
    return http_send_request("POST", url , param, header, crypto); 
}
 
typedef struct _AsyncUserData{
    gchar *url ; 
    gchar *eventName ; 
    gpointer eventUserData;
    SoupMessage *msg ; 
}AsyncUserData; 


static void http_task_data_free(gpointer d){
    AsyncUserData *data = (AsyncUserData *)d ;
    if (!data) return ; 

    if (data->url){
        g_free(data->url); 
        data->url = NULL ; 
    } 

    if (data->eventName){
        g_free(data->eventName); 
        data->eventName = NULL ; 
    }

    if (data->msg){
        g_object_unref(data->msg); 
        data->msg = NULL ;      
    }
    g_free(data); 
}

static void http_task_response_free(gpointer r){
    Response *resp = (Response *)r ; 
    if (!resp) return ; 
    delete resp ; 
}

static void http_send_post_async_task_callback (GObject *source_object,
				     GAsyncResult *result,
				     gpointer user_data){
    Response *resp = NULL ; 
    AsyncUserData *data = NULL ; 
    GTask* task = NULL ; 
    GError *error = NULL ; 
    std::string evtName ; 
    AsyncResult resultParam ;
    if (!g_task_is_valid(result, source_object)) goto END ; 

    task = G_TASK(result); 
    if (task == NULL) goto END ;

    data = (AsyncUserData *)g_task_get_task_data(task); 
    if (!data) goto END ;


    if (data->eventName){
        evtName = data->eventName; 
    }

    resp = (Response *)g_task_propagate_pointer(task, &error);

    if (resp == NULL){
        resp = new (std::nothrow) Response(data->url); 
        if (resp){
            if (error){
                resp->setResponse(error->message); 
            }
        }
    }


END : 

    if (error){
        g_error_free(error); 
        error = NULL ; 
    }

    if (!evtName.empty()){
        resultParam.resp = resp;
        resultParam.user_data = data->eventUserData;
        EventManager::getInstance()->dispatchEvent(evtName.c_str(), &resultParam); 
    }

    if (resp){
        http_task_response_free(resp); 
        resp = NULL ; 
    }
}


static void
http_send_post_async_task_in_thread (GTask        *task,
                   gpointer      source_object,
                   gpointer      task_data,
                   GCancellable *cancellable)
{
    AsyncUserData *user_data = (AsyncUserData *)task_data ; 
    GError *error = NULL;
    SoupSession *session = NULL ; 
    SoupMessage *msg = NULL ; 
    SoupURI *uri = NULL ;
    guint status ;  
    Response *rv = NULL ; 
    JsonParser *parser = NULL;
    JsonNode *node = NULL ; 

    if (!user_data){
        error = g_error_new(SOUP_HTTP_ERROR, SOUP_STATUS_BAD_REQUEST, "AsyncUserData is NULL"); 
        goto END ; 
    }

    session =(SoupSession *) source_object; 
    if (!session){
        error = g_error_new(SOUP_HTTP_ERROR, SOUP_STATUS_BAD_REQUEST, "SoupSession is NULL"); 
        goto END ; 
    }

    g_object_ref(session); 

    msg = user_data->msg ; 
    if (!msg){
        error = g_error_new(SOUP_HTTP_ERROR, SOUP_STATUS_BAD_REQUEST, "SoupMessage is NULL"); 
        goto END ; 
    }

    g_object_ref(msg); 

    rv = new(std::nothrow) Response(); 

    status = soup_session_send_message(session, msg); 

    rv->setStatus(status); 
    
    soup_message_headers_foreach(msg->response_headers, http_set_message_header, rv);
    if (SOUP_STATUS_IS_SUCCESSFUL(status)){
        read_response_cookies(msg->response_headers); 
        parser = json_parser_new(); 
        if (!parser) {
            error = g_error_new(SOUP_HTTP_ERROR, SOUP_STATUS_BAD_REQUEST, "JsonParser is NULL"); 
            goto END ; 
        }

        if (json_parser_load_from_data(parser, msg->response_body->data, msg->response_body->length, NULL)) {
            node = json_parser_get_root(parser); 
            if (node){
                node = json_node_copy(node); 
            }
        }else {
            rv->setResponse(msg->response_body->data, msg->response_body->length); 
        }
    }

END : 
    if (parser)
    {
        g_object_unref(parser);
        parser = NULL;
    }

    if (session){
        g_object_unref(session); 
        session = NULL ; 
    }

    if (msg)
    {
        g_object_unref(msg);
        msg = NULL;
    }

    if (node){
        rv->setJsonNode(node); 
        json_node_unref(node); 
    }

    if (error){
        g_task_return_error(task, error); 
        error = NULL ; 
        if (rv){
            delete rv ; 
            rv = NULL ; 
        }
    }else if (rv){
        g_task_return_pointer(task, rv , http_task_response_free); 
    }else {
        error = g_error_new(SOUP_HTTP_ERROR, SOUP_STATUS_BAD_REQUEST, "Response is NULL"); 
        g_task_return_error(task, error);
    }
}

bool http_send_post_async_task(const char *url , const char *eventName , gpointer eventUserData , HTTP_PARAM *param  ,  const HTTP_HEADER *header,  bool crypto){
    bool rv = false ; 
    SoupSession *session = NULL;
    SoupMessage *msg = NULL;
    AsyncUserData *user_data = NULL ; 
    GTask *task = NULL ; 

    session = http_get_session(); 
    if (!session) return rv ; 

    msg = http_create_message("POST", url , param, header, crypto); 

    if (!msg)
        goto END;

    user_data = (AsyncUserData *)g_new0(AsyncUserData , 1); 
    if (!user_data) goto END; 

    user_data->url = g_strdup(url); 
    user_data->msg = msg ; 
    g_object_ref(user_data->msg); 

    if (eventName){
        user_data->eventName = g_strdup(eventName); 
        user_data->eventUserData = eventUserData;
    }

    task = g_task_new(session, NULL, http_send_post_async_task_callback, NULL); 

    if (!task){
        goto END ; 
    }
    g_task_set_task_data(task, user_data, http_task_data_free); 
    g_task_run_in_thread(task, http_send_post_async_task_in_thread); 
    rv = true ; 
END : 
    if (msg){
        g_object_unref(msg);
        msg = NULL;
    }

    if (task){
        g_object_unref(task); 
        task = NULL ; 
    }
    
    return rv ; 
}

std::shared_ptr<Response> http_send_file_multipart(const char *url , const char *filename , unsigned char **content , long long size , long long offset, long long totalSize,  const HTTP_PARAM *param ,  const HTTP_HEADER *header,  bool crypto){
    auto rv = std::make_shared<Response>(url) ; 
    SoupMultipart *multipart = NULL ; 
    SoupBuffer *soupBuffer = NULL ;
    JsonNode *node = NULL;
    SoupSession *session = NULL;
    SoupMessage *msg = NULL;
    GError *error = NULL;
    JsonParser *parser = NULL;
    int status = 0 , read_len = 0, index = 0 ; 
    std::string ftc_url = FTC_URL_DEFINE;

    if (ftc_url.empty() || url == NULL || filename == NULL || content == NULL || *content == NULL) return rv ; 

    ftc_url.append(url); 

    session = http_get_session(); 

    if (!session) return rv ; 

    multipart = soup_multipart_new(MULTI_PART); 

    if (!multipart) goto END ; 

    if (param != NULL){
        for(auto it = param->begin() ; it != param->end() ; it++){
            std::string value;
            const char *s = it->second.c_str(); 
            value = s;
            soup_multipart_append_form_string(multipart, it->first.c_str(), value.c_str()) ; 
        }
    }

    soupBuffer = soup_buffer_new(SOUP_MEMORY_TEMPORARY, *content , size); 
    soup_multipart_append_form_file(multipart, "files[]", filename, OCTET_STREAM, soupBuffer); 

    msg = soup_message_new("POST", ftc_url.c_str()); 
    if (!msg) goto END ; 

    soup_multipart_to_message(multipart, msg->request_headers, msg->request_body); 

    if (size != totalSize) {
        std::stringstream ss ; 
        ss << "bytes " << offset << "-" << (offset + size -1) << "/" << totalSize ; 
        soup_message_headers_append(msg->request_headers, "Content-Range", ss.str().c_str()); 
    }
    
    status = soup_session_send_message(session, msg); 

    rv->setStatus(status); 
    if (SOUP_STATUS_IS_SUCCESSFUL(status)){
        read_response_cookies(msg->response_headers); 
        parser = json_parser_new(); 
        if (!parser) goto END ; 
        if (json_parser_load_from_data(parser, msg->response_body->data, msg->response_body->length, &error)) {
            node = json_parser_get_root(parser); 
            if (node){
                node = json_node_copy(node); 
            }
        }else {
            rv->setResponse(msg->response_body->data, msg->response_body->length); 
        }
    }

END:
    if (soupBuffer) {
        soup_buffer_free(soupBuffer);
        soupBuffer = NULL ;
    }

    if (*content){
        free(*content); 
        *content = NULL ; 
    }

    if (parser)
    {
        g_object_unref(parser);
        parser = NULL;
    }

    if (msg)
    {
        g_object_unref(msg);
        msg = NULL;
    }

    if (node){
        rv->setJsonNode(node); 
        json_node_unref(node); 
    }
    

    if (multipart){
        soup_multipart_free(multipart); 
    }

    if (error) {
        g_error_free(error);
    }

    return rv;
} 

std::shared_ptr<Response> http_send_file_stream(const char *url, const char *filename, unsigned char **content , long long size, const char *type, const char *enckey, const HTTP_PARAM *param, const HTTP_HEADER *header, bool crypto) {
    auto rv = std::make_shared<Response>(url) ;
    SoupMultipart *multipart = NULL;
    SoupBuffer *soupBuffer = NULL;
    SoupMessage *msg = NULL;
    SoupSession *session = NULL;
    JsonNode *node = NULL;
    JsonParser *parser = NULL;
    GError *error = NULL;
    int status = 0;
    std::string ftc_url = FTC_URL_DEFINE;

    if (ftc_url.empty() || url == NULL || content == NULL || size <= 0) {
        return rv;
    }

    ftc_url.append(url);

    session = http_create_session();
    if (session == NULL) {
        return rv;
    }

    multipart = soup_multipart_new(MULTI_PART);

    if (!multipart) goto END;

    //  파라미터
    //
    if (param != NULL) {
        for (auto it = param->begin(); it != param->end(); it++) {          
            soup_multipart_append_form_string(multipart, it->first.c_str(), it->second.c_str());
        }
    }

    soupBuffer = soup_buffer_new(SOUP_MEMORY_TEMPORARY, *content, size);
    //  파일이름에 한글이 입력되면 파일전송이 되지않음
    soup_multipart_append_form_file(multipart, "file", filename, type, soupBuffer);

    msg = soup_message_new("POST", ftc_url.c_str());
    if (!msg) {
        goto END;
    }

    soup_multipart_to_message(multipart, msg->request_headers, msg->request_body);

    status = soup_session_send_message(session, msg);

    rv->setStatus(status); 
    if (SOUP_STATUS_IS_SUCCESSFUL(status)){
        parser = json_parser_new(); 
        if (!parser) goto END ; 
        if (json_parser_load_from_data(parser, msg->response_body->data, msg->response_body->length, &error)) {
            node = json_parser_get_root(parser); 
            if (node) {
                node = json_node_copy(node); 
            }
        }else {
            rv->setResponse(msg->response_body->data, msg->response_body->length); 
        }
    }

END:
    if (multipart) {
        soup_multipart_free(multipart); 
        multipart = NULL ; 
    }

    if (soupBuffer) {
        soup_buffer_free(soupBuffer);
        soupBuffer = NULL ;
    }

    if (*content) {
        delete[] (*content); 
        *content = NULL ; 
    }

    if (parser)
    {
        g_object_unref(parser);
        parser = NULL;
    }

    if (msg)
    {
        g_object_unref(msg);
        msg = NULL;
    }

    if (node){
        rv->setJsonNode(node); 
        json_node_unref(node); 
    }

    if (error) {
        g_error_free(error);
    }

    if (session){
        soup_session_abort(session); 
        g_clear_object(&session); 
        session = NULL ; 
    }


    return rv;
}

bool http_download_file(const char *url, const HTTP_PARAM *param, const HTTP_HEADER *header, const char *filepath, bool crypto) {
    auto resp = std::make_shared<Response>(url) ;
    bool rv = false;
    SoupSession *session = NULL;
    SoupMessage *msg = NULL;
    SoupURI *uri = NULL;
    GInputStream *istream = NULL;
    GFileOutputStream *fostream = NULL;
    GFile *file = NULL, *dir;
    GError *error = NULL;
    FileInfo fi;
    char buffer[4096] = { 0, };
    gsize read_byte = 0, total_read_bytes = 0, content_length;
    int status = 0 ; 

    if (url == NULL || filepath == NULL) {
        return rv;
    }

    fi = FileInfo::getFileInfo(filepath);

    session = http_get_session(); 
    if (!session)  {
        return rv ; 
    }

    msg = http_create_message("POST", url , param, header, crypto); 

    if (!msg) {
        goto END;
    }
    
    istream = soup_session_send(session, msg, NULL, &error); 
    if (!istream) {
        goto END;
    }

    if (msg){
        SoupMessageHeaders *headers = msg->response_headers; 
        if (headers != NULL){
            soup_message_headers_foreach(msg->response_headers, http_set_message_header, resp.get());
            content_length = StringUtils::getInt(resp->getResponseHeaderValue("Content-Length"));
        }
    }

    //  디렉터리가 있는지 검사하고, 없다면 생성해준다.
    dir = g_file_new_for_path(fi.getPath().c_str());
    if (!dir) {
        goto END;
    }
    if (!g_file_query_exists(dir, NULL)) {
        if (!g_file_make_directory_with_parents(dir, NULL, &error)) {
            goto END;
        }
    }

    file = g_file_new_for_path(fi.getFullPath().c_str());
    if (!file) {
        goto END;
    }

    //  파일이 있는지 검사하고, 있다면 삭제한다.
    if (g_file_query_exists(file, NULL)) {
        if (!g_file_delete(file, NULL, &error)) {
            goto END;
        }
    }

    fostream = g_file_create(file, G_FILE_CREATE_NONE, NULL, &error);
    if (!fostream) {
        goto END;
    }

    do {
        read_byte = g_input_stream_read(istream, buffer, 4096, NULL, &error);
        if (read_byte <= 0) {
            break;
        }

        g_output_stream_write(G_OUTPUT_STREAM(fostream), buffer, read_byte, NULL, &error);

        total_read_bytes += read_byte;
    } while (true);

    rv = true;
END:
    if (istream) {
        g_input_stream_close(G_INPUT_STREAM(istream), NULL, NULL);
        g_object_unref(istream);
        istream = NULL;
    }

    if (fostream) {
        g_output_stream_close(G_OUTPUT_STREAM(fostream), NULL, NULL);
        g_object_unref(fostream);
        fostream = NULL;
    }

    if (msg) {
        g_object_unref(msg);
        msg = NULL;
    }

    if (error){
        if (g_file_query_exists(file, NULL) == true) {
            g_file_delete(file, NULL, NULL);
        }

        g_error_free(error); 
        error = NULL ; 
    }

    if (dir) {
        g_object_unref(dir);
        dir = NULL;
    }

    if (file) {
        g_object_unref(file);
        file = NULL;
    }

    return rv;
}

int http_dispose(){
    int rv = 0 ; 

    http_clear_cookies(); 
    
    if (!s_cookies.empty()){
        s_cookies.clear(); 
    }

    

    if (s_session){
        soup_session_abort(s_session); 
        g_clear_object(&s_session); 
        s_session = NULL ; 
    }

    rv = 1 ; 
    return rv ; 
}