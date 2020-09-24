#ifndef __FTC_CORE_CLIPBOARD_H__
#define __FTC_CORE_CLIPBOARD_H__
#include <gtk/gtk.h>
#include <map>
#include "macro.h"
#include "EventManager.h"
#include "Event.h"
#include "model/ValueObject.h"

BEGIN_FTC_CORE

typedef struct _ClipboardFormat
{
    int length;
    char *data;

    _ClipboardFormat()
    {
        length = 0;
        data = nullptr;
    }
} ClipboardFormat;

class Clipboard{
public:
    static Clipboard *getInstance(); 
    static void destroyInstance(); 
    virtual ~Clipboard(); 

    enum FORMAT {
        DONE,
        IMAGE,
        RICH_TEXT,
        TEXT,
        CSHARP_TEXT
    };

public:
    FORMAT getFormat();
    FORMAT get(char **buffer, unsigned int *len);
    bool send(const char *to_network);
    bool set(std::map<std::string, ClipboardFormat> &dataFormat);
    bool set(FORMAT format, char *buffer, unsigned int len);
    bool loadFile(const char *filepath);
    
    void setMainWindow(GtkWidget *widget);
private: 
    Clipboard();
    void dispose();

    static void sendThread(char *toNetwrokIp, char *sendData, unsigned int dataLen);

    bool setImageData(const char *buffer, unsigned int len);
    bool setRichTextData(const char *buffer, unsigned int len);
    bool setTextData(const char *buffer, unsigned int len);

    static void setImageDataEvent(Event *evt);
    static void setRichTextDataEvent(Event *evt);
    static void setTextDataEvent(Event *evt);

    bool getImageData(char **buffer, unsigned int *len);
    bool getRichTextData(char **buffer, unsigned int *len);
    bool getTextData(char **buffer, unsigned int *len);

    ClipboardFormat readFormatData(GInputStream *istream);
private: 
    static Clipboard *s_instance ; 
    GtkWidget *_main_window ;
    
}; 

END_FTC_CORE

#endif