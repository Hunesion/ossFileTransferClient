#include "Clipboard.h"
#include "Logger.h"
#include "StringUtils.h"
#include "TimeUtils.h"
#include "FileUtils.h"
#include "Request.h"
#include "JsonUtils.h"
#include "CommonUtils.h"
#include "model/GlobalVar.h"

#include <memory>
#include <thread>

#define HUNE_CLIPBOARD_SIGNATURE                "HNSCLIP"
#define CLIPBOARD_FORMAT_IMAGE_PUBLIC_TIFF      "public.tiff"
#define CLIPBOARD_FORMAT_IMAGE_BITMAP           "Bitmap"
#define CLIPBOARD_FORMAT_TEXT_CSHARP            "Text"
#define CLIPBOARD_FORMAT_TEXT_PLAIN             "text/plain"


BEGIN_FTC_CORE

typedef struct _ClipboardSetTextParam
{
    GtkClipboard *cb;
    char *data;
    unsigned int len;
} ClipboardSetTextParam;

typedef struct _ClipboardSetImageParam
{
    GtkClipboard *cb;
    GdkPixbuf *pixbuf;
} ClipboardSetImageParam;


Clipboard* Clipboard::s_instance = nullptr ;

Clipboard *Clipboard::getInstance() {
    Clipboard* rv = Clipboard::s_instance ; 
    if (!rv){
        rv = new(std::nothrow)Clipboard(); 
        Clipboard::s_instance = rv ; 
    }
    return rv ; 
}

void Clipboard::destroyInstance() {
    
    if (Clipboard::s_instance){
        delete Clipboard::s_instance; 
        Clipboard::s_instance = nullptr ; 
    }

}

Clipboard::Clipboard()
    : _main_window(nullptr)
{
    EventManager *event_mgr = EventManager::getInstance();

    if (event_mgr != nullptr) {
        event_mgr->addEventListener(this, FTC_CORE_CLIPBOARD_SET_TEXT, HUNE_CALLBACK_CLS_STATIC_1(Clipboard::setTextDataEvent));
        event_mgr->addEventListener(this, FTC_CORE_CLIPBOARD_SET_RICHTEXT, HUNE_CALLBACK_CLS_STATIC_1(Clipboard::setRichTextDataEvent));
        event_mgr->addEventListener(this, FTC_CORE_CLIPBOARD_SET_IMAGE, HUNE_CALLBACK_CLS_STATIC_1(Clipboard::setImageDataEvent));
    }
}

Clipboard::~Clipboard() {
    this->dispose();
} 

void Clipboard::dispose() {
    EventManager *event_mgr = EventManager::getInstance();
    if (event_mgr != nullptr) {
        event_mgr->removeEventListener(this);
    }

    if (this->_main_window) {
        g_object_unref(this->_main_window);
        this->_main_window = nullptr;
    }
}

void Clipboard::setMainWindow(GtkWidget *widget) {
    if (this->_main_window != nullptr) {
        g_object_unref(this->_main_window);
    }
    g_object_ref(widget);
    this->_main_window = widget;
}

Clipboard::FORMAT Clipboard::getFormat() {
    GtkClipboard *cb = nullptr;
    FORMAT rv = FORMAT::DONE;
    GtkTextBuffer *textBuffer = nullptr;

    cb = gtk_widget_get_clipboard(this->_main_window, GDK_SELECTION_CLIPBOARD);
    if (cb == nullptr) {
        return rv;
    }

    if (gtk_clipboard_wait_is_image_available(cb) == true) {
        rv = FORMAT::IMAGE;
    } else if (gtk_clipboard_wait_is_text_available(cb) == true) {
        rv = FORMAT::TEXT;
    } else {
        textBuffer = gtk_text_buffer_new(nullptr);
        if (gtk_clipboard_wait_is_rich_text_available(cb, textBuffer) == true) {
            rv = FORMAT::RICH_TEXT;
        }
    }
    
    if (textBuffer != nullptr) {
        g_object_unref(textBuffer);
        textBuffer = nullptr;
    }

    return rv ;
}

Clipboard::FORMAT Clipboard::get(char **buffer, unsigned int *len)
{
    FORMAT rv = FORMAT::DONE;

    FTC_LOG("Clipboard::get IN");
    rv = this->getFormat();
    
    switch (rv)
    {
        case FORMAT::IMAGE :
            {
                this->getImageData(buffer, len);
            }
            break;
        case FORMAT::RICH_TEXT :
            {
                //  미지원
                FTC_LOG_DEBUG("RICH_TEXT");
            }
            break;
        case FORMAT::TEXT :
            {
                this->getTextData(buffer, len);
            }
            break;
        default:
            {
                FTC_LOG("invalid format");
            }
            break;
    }
    FTC_LOG("Clipboard::get OUT");
    return rv;
}

void Clipboard::sendThread(char *toNetwrokIp, char *sendData, unsigned int dataLen)
{
    std::string filename, msg, code;
    std::string message;
    std::tm tm = TimeUtils::now();   
    const char *err = NULL;
    JsonNode *node = NULL;
    bool rv = false;
    
    try
    {
        if (! toNetwrokIp || ! sendData || sendData <= 0) {
            throw std::runtime_error("");
        }

        filename = "clipboard";

        auto resp = Request::requestFileUploadApi(filename.c_str(), (unsigned char**)&sendData, dataLen, NULL, toNetwrokIp);   
        if (! resp->isSuccess()) {
            throw std::runtime_error("UploadApi HTTP 오류: " + resp->getResponseMessage());
        }

        CommonUtils::callNotificationEvent("클립보드를 전송하였습니다.");

        rv = true;
    } catch(const std::runtime_error& e) {
        err = e.what();

        if (strlen(err) > 0) {
            FTC_LOG("클립보드 전송 실패 : %s", err);
            message = err;
            StringUtils::replace(message, "\\n", "\n");
            CommonUtils::callMessageBoxEvent(message);
        }
    }

    if (sendData != nullptr) {
        delete[] sendData;
        sendData = nullptr;
    }

    if (toNetwrokIp != nullptr) {
        g_free(toNetwrokIp);
        toNetwrokIp = nullptr;
    }
}

bool Clipboard::send(const char *to_network_ip)
{
    std::thread thread;
    Clipboard::FORMAT format = FORMAT::DONE;
    char *data = nullptr;
    unsigned int dataLen = 0;
    gchar *allocToNetowrkIp = nullptr;
    std::string message;
    const char *err = NULL;
    bool rv = false;

    if (! to_network_ip) {
        return rv;
    }

    try
    {
        format = this->get(&data, &dataLen);
        if (format == FORMAT::DONE || data == NULL || dataLen <= 0) {
            //  클립보드를 읽어내지 못하여 전송할 수 없습니다.
            throw std::runtime_error("클립보드를 읽어내지 못하여 전송할 수 없습니다.");
        }
        allocToNetowrkIp = g_strdup(to_network_ip);

        thread = std::thread(Clipboard::sendThread, allocToNetowrkIp, data, dataLen);
        thread.detach();
    } catch(const std::runtime_error& e) {
        err = e.what();

        if (strlen(err) > 0) {
            FTC_LOG("클립보드 전송 실패 : %s", err);

            message = err;
            StringUtils::replace(message, "\\n", "\n");
            CommonUtils::callMessageBoxEvent(message);
        }
    }

    if (data != nullptr) {
        delete[] data;
        data = nullptr;
    }
    
    rv = true;
    return rv;
}

bool Clipboard::set(std::map<std::string, ClipboardFormat> &dataFormat) {
    bool rv = false;
    FORMAT format = FORMAT::DONE;


    for (auto &it : dataFormat) {
        if (it.first == CLIPBOARD_FORMAT_TEXT_PLAIN/*리눅스*/) {
            format = FORMAT::TEXT;
        } else if (it.first == CLIPBOARD_FORMAT_TEXT_CSHARP/*WIN, MAC*/) {
            format = FORMAT::CSHARP_TEXT;
        } else if (it.first == CLIPBOARD_FORMAT_IMAGE_BITMAP /* WIN */ || it.first == CLIPBOARD_FORMAT_IMAGE_PUBLIC_TIFF /* MAC */ ) {
            format = FORMAT::IMAGE;
        }

        if (format != FORMAT::DONE) {
            this->set(format, it.second.data, it.second.length);
            break;
        }
    }
}

bool Clipboard::set(FORMAT format, char *buffer, unsigned int len) {
    bool rv = false;
    const char *str = nullptr;

    switch (format)
    {
    case FORMAT::IMAGE :
        {
            rv = this->setImageData(buffer, len);
        }
        break;
    case FORMAT::RICH_TEXT :
        {
            //  미지원
            FTC_LOG_DEBUG("RICH_TEXT");
        }
        break;
    case FORMAT::CSHARP_TEXT :
        {
            std::string text = (buffer + 23);
            StringUtils::trim(text);
            StringUtils::trim(text, '\002');
            StringUtils::trim(text, '\001');
            str = text.c_str();
            rv = this->setTextData(str, strlen(str));
        }
        break;
    case FORMAT::TEXT :
        {   
            rv = this->setTextData(buffer, strlen(buffer));   
        }
        break;
    default:
        return rv;
    }

    if (rv == true) {
        //  클립보드가 수신되었습니다.
        CommonUtils::callNotificationEvent("클립보드가 수신되었습니다.");
    }

    return rv;
}

bool Clipboard::setImageData(const char *buffer, unsigned int len)
{
    GdkPixbuf *pixbuf = nullptr;
    EventManager *event_mgr = nullptr;
    ClipboardSetImageParam *param = nullptr;
    std::string path;
    std::tm tm = TimeUtils::now();            
    bool rv = false;

    if (buffer == nullptr) {
        return rv;
    }

    if (len <= 0) {
        return rv;
    }

    path = "/tmp/" + TimeUtils::getDateFormat(tm, "%Y%m%d%H%M%S") + ".png";
    if (FileUtils::fileWrite(path.c_str(), (char*)buffer, len) == false) {
        return rv;
    }
    
    pixbuf = gdk_pixbuf_new_from_file(path.c_str(), NULL);
    if (! pixbuf) {
        return rv;
    }

    event_mgr = EventManager::getInstance();
    if (event_mgr == nullptr) {
        return rv;
    }

    param = new ClipboardSetImageParam();
    memset(param, 0, sizeof(ClipboardSetImageParam));

    param->cb = gtk_widget_get_clipboard(this->_main_window, GDK_SELECTION_CLIPBOARD);
    param->pixbuf = pixbuf;

    EventManager::getInstance()->dispatchEventAsync(FTC_CORE_CLIPBOARD_SET_IMAGE, (void*)param, [](void* release){
        ClipboardSetImageParam *param = (ClipboardSetImageParam*)release;

        if (param != nullptr) {
            if (param->pixbuf != nullptr) {
                g_object_ref(param->pixbuf);
            }

            delete param;
        }
    });

    FileUtils::fileDelete(path.c_str());

    rv = true;
    return rv;
}

bool Clipboard::setTextData(const char *buffer, unsigned int len)
{
    EventManager *event_mgr = nullptr;
    ClipboardSetTextParam *param = nullptr;
    char *textData = nullptr;
    bool rv = false;

    if (buffer == nullptr) {
        return rv;
    }
    if (len <= 0) {
        return rv;
    }

    event_mgr = EventManager::getInstance();
    if (event_mgr == nullptr) {
        return rv;
    }

    textData = new char[len + 1];
    memset(textData, 0, len + 1);
    memcpy(textData, buffer, len);

    param = new ClipboardSetTextParam();
    memset(param, 0, sizeof(ClipboardSetTextParam));

    param->cb = gtk_widget_get_clipboard(this->_main_window, GDK_SELECTION_CLIPBOARD);
    param->data = textData;
    param->len = len;
    
    EventManager::getInstance()->dispatchEventAsync(FTC_CORE_CLIPBOARD_SET_TEXT, (void*)param, [](void* release){
        ClipboardSetTextParam *param = (ClipboardSetTextParam*)release;

        if (param != nullptr) {
            if (param->data != nullptr) {
                delete[] param->data;
            }

            delete param;
        }
    });
    rv = true;
    return rv;
}

void Clipboard::setImageDataEvent(Event *evt)
{
    ClipboardSetImageParam *param = nullptr;

    if (evt == nullptr) {
        return;
    }

    param = (ClipboardSetImageParam*)evt->getUserData();
    if (param == nullptr) {
        return;
    }

    if (param->cb == nullptr || param->pixbuf == nullptr) {
        return;
    }

    gtk_clipboard_set_image(param->cb, param->pixbuf);
}

void Clipboard::setTextDataEvent(Event *evt)
{
    ClipboardSetTextParam *param = nullptr;

    if (evt == nullptr) {
        return;
    }

    param = (ClipboardSetTextParam*)evt->getUserData();
    if (param == nullptr) {
        return;
    }

    if (param->cb == nullptr || param->data == nullptr || param->len <= 0) {
        return;
    }

    gtk_clipboard_set_text(param->cb, (const char*)param->data, param->len);
}

void Clipboard::setRichTextDataEvent(Event *evt)
{

}


bool Clipboard::getImageData(char **buffer, unsigned int *len) {
    GtkClipboard *cb = nullptr;
    GdkPixbuf *pixbuf = nullptr;
    GError *error = nullptr;
    bool rv = false;
    
    cb = gtk_widget_get_clipboard(this->_main_window, GDK_SELECTION_CLIPBOARD);
    if (cb == nullptr) {
        return rv;
    }

    pixbuf = gtk_clipboard_wait_for_image(cb);
    if (pixbuf == nullptr) {
        return rv;
    }

    if (gdk_pixbuf_save_to_buffer(pixbuf, buffer, (gsize*)len, "png", &error, NULL) == false) {
        return rv;
    }

    if (pixbuf != nullptr) {
        g_object_unref(pixbuf);
        pixbuf = nullptr;
    }

    if (error != nullptr) {
        g_error_free(error);
        error = nullptr;
    }

    rv = true;
    return rv;
}

bool Clipboard::getRichTextData(char **buffer, unsigned int *len) {
    GtkClipboard *cb = nullptr;
    bool rv = false;

    cb = gtk_widget_get_clipboard(this->_main_window, GDK_SELECTION_CLIPBOARD);
    if (cb == nullptr) {
        return rv;
    }
}

bool Clipboard::getTextData(char **buffer, unsigned int *len) {
    GtkClipboard *cb = nullptr;
    gchar *text = nullptr;
    unsigned int textLen = 0;
    bool rv = false;

    if (len == nullptr) {
        return rv;
    }

    cb = gtk_widget_get_clipboard(this->_main_window, GDK_SELECTION_CLIPBOARD);
    if (cb == nullptr) {
        return rv;
    }

    text = gtk_clipboard_wait_for_text(cb);
    if (text == nullptr) {
        return rv;
    }

    textLen = strlen(text);

    *buffer = new char[textLen + 1];
    memset(*buffer, 0, textLen + 1);
    memcpy(*buffer, text, textLen);

    if (len != nullptr) {
        *len = textLen;
    }

    if (text != nullptr) {
        g_free(text);
        text = nullptr;
    }

    rv = true;
    return rv;
}

bool Clipboard::loadFile(const char *filepath)
{
    std::map<std::string, ClipboardFormat> formatDataDic;
    GFile *file = nullptr;
    GInputStream *istream = nullptr;
    GError *error = nullptr;
    int totalCount = 0, private_header_size = 0;
    gsize readBytes = 0;
    bool rv = false;

    if (! filepath) {
        return rv;
    }

    file = g_file_new_for_path(filepath);
    if (file == nullptr) {
        return rv;
    }

    try {
        istream = G_INPUT_STREAM(g_file_read(file, NULL, &error));
        if (istream == nullptr) {
            throw std::runtime_error("");
        }
        
        ClipboardFormat dataFormat;
        char *format = nullptr;

        //  클립보드 포멧
        //
        dataFormat = readFormatData(istream);
        format = dataFormat.data;

        dataFormat = readFormatData(istream);
        if (dataFormat.data != nullptr) {
            formatDataDic.insert(std::pair<std::string, ClipboardFormat>(std::string(format), dataFormat));
        }

        if (format != nullptr) {
            delete[] format;
        }

        this->set(formatDataDic);

        rv = true;
    } catch(std::runtime_error ex) {
        if (error != nullptr) {
            FTC_LOG("Fail Clipboard loadfile : %s", error->message);
            g_error_free(error);
            error = nullptr;
        }
    }

    if (g_file_delete(file, NULL, &error) == false ) {
        FTC_LOG("file delete fail %s \n", filepath);
    }

    if (istream != nullptr) {
        g_input_stream_close(istream, nullptr, nullptr);
        g_object_unref(istream);
        istream = nullptr;
    }

    if (file != nullptr) {
        g_object_unref(file);
        file = nullptr;
    }

    if (error != nullptr) {
        g_error_free(error);
        error = nullptr;
    }

    for (auto &it : formatDataDic) {
        if (it.second.data != nullptr) {
            delete[] it.second.data;
            it.second.data = nullptr;
        }
    }
    
    return rv;
}

ClipboardFormat Clipboard::readFormatData(GInputStream *istream)
{
    ClipboardFormat rv;
    GError *error = nullptr;
    gsize readBytes = 0;

    readBytes = g_input_stream_read(istream, (void*)&rv.length, 4, NULL, &error);
    if (readBytes < 4) {
        return rv;
    }

    rv.data = new char[rv.length + 1];
    memset(rv.data, 0, rv.length + 1);

    readBytes = g_input_stream_read(istream, rv.data, rv.length, NULL, &error);

    if (error != nullptr) {
        g_error_free(error);
        error = nullptr;
    }
    
    return rv;
}

END_FTC_CORE