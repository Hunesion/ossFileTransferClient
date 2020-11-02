#include "UrlRedirection.h"
#include "FileUtils.h"
#include "TimeUtils.h"
#include "Request.h"
#include "sys_utils.h"
#include "Logger.h"
#include "JsonUtils.h"
#include "CommonUtils.h"
#include "model/GlobalVar.h"
#include "model/Properties.h"

#include <thread>


BEGIN_FTC_CORE

bool openUrl(const char *url)
{
    std::string command, urlStr;

    if (! url) {
        return false;
    }

    urlStr = url;

    if ((urlStr.find("http://") == std::string::npos) && (urlStr.find("https://") == std::string::npos)) {
        urlStr = urlStr.insert(0, "http://");
    }

    return !ftc_open_url(urlStr);
}

bool loadUrl(const char *filepath)
{
    std::string readStr, url, command;
    std::string message;
    EventManager *event_mgr = EventManager::getInstance();
    bool rv = false;

    if (! filepath) {
        goto END;
    }

    readStr = FileUtils::fileReadString(filepath);
    if (readStr.length() <= 0) {
        goto END;
    }

    FileUtils::fileDelete(filepath);

    url = readStr;
    
    if (openUrl(url.c_str()) == false) {
        goto END;
    }

    rv = true;
END:
    if (event_mgr != nullptr) {
        if (rv) {
            //  URL이 수신되었습니다.
            message = "URL이 수신되었습니다.";
        } else {
            //  처리할 수 없는 url입니다.
            message = "처리할 수 없는 url입니다.";
        }

        CommonUtils::callNotificationEvent(message);
    }

    return rv;
}

static void sendUrlThread(char *url)
{
    std::string filename;
    char *buffer = NULL;
    const char *err = NULL;
    bool rv = false;
    
    try {
        filename = "urlredirect";
        buffer = new char[strlen(url) + 1];
        memset(buffer, 0, strlen(url) + 1);
        memcpy(buffer, url, strlen(url));

        auto resp = Request::requestFileUploadApi(filename.c_str(), (unsigned char**)&buffer, strlen(url), NULL, NULL);
        if (! resp->isSuccess()) {
            throw std::runtime_error("UploadApi HTTP 오류: " + resp->getResponseMessage());
        }

        CommonUtils::callNotificationEvent("URL Redirection을 전송하였습니다.");
    } catch(const std::runtime_error& e) {
        err = e.what();

        if (strlen(err) > 0) {
            FTC_LOG("URL Redirection 전송 실패 : %s", err);
            
            CommonUtils::callMessageBoxEvent(err);
        }
    }

    if (buffer) {
        delete[] buffer;
        buffer = NULL;
    }

    if (url) {
        g_free(url);
        url = NULL;
    }
}

bool sendUrl(const char *url)
{
    std::thread thread;
    gchar *alloc_url = NULL;
    bool rv = false;

    if (! url) {
        return rv;
    }

    alloc_url = g_strdup(url);

    thread = std::thread(sendUrlThread, alloc_url);
    thread.detach();

    rv = true;
    return rv;
}


END_FTC_CORE

