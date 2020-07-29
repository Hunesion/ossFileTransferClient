#include "ftchomeheadersendbox.h"



struct _FtcHomeHeaderSendBox
{
    GtkBox parent;
};

typedef struct _FtcHomeHeaderSendBoxPrivate FtcHomeHeaderSendBoxPrivate;

struct _FtcHomeHeaderSendBoxPrivate
{
    FtcHomePage *home_page;

    GtkLabel    *lbl_send;

    GtkEventBox *event_box_send_clipboard;
    GtkEventBox *event_box_send_url;
    GtkEventBox *event_box_copy_cert;

    GtkImage   *imgbtn_send_clipboard;
    GtkImage   *imgbtn_send_url;
    GtkImage   *imgbtn_copy_cert;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomeHeaderSendBox, ftc_home_header_send_box, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_header_send_box_on_button_press_event_box_send_clipboard(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_home_header_send_box_on_button_press_event_box_send_url(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_home_header_send_box_on_activate_clipboard_selection(GtkWidget *widget, gpointer data);
///////////////////////////////////////////////////////////////////

static void
ftc_home_header_send_box_init (FtcHomeHeaderSendBox *box)
{
    FtcHomeHeaderSendBoxPrivate *priv = NULL;
    bool margin = false;

    gtk_widget_init_template (GTK_WIDGET (box));
    priv = (FtcHomeHeaderSendBoxPrivate*)ftc_home_header_send_box_get_instance_private (box);

    //  Todo..

    //  Signal (Event) 함수 연결
    //  imgbtn_send_clipboard(event_box_send_clipboard) click
    g_signal_connect(priv->event_box_send_clipboard, "button-press-event", G_CALLBACK(ftc_home_header_send_box_on_button_press_event_box_send_clipboard), box);

    //  imgbtn_send_url(event_box_send_url) click
    g_signal_connect(priv->event_box_send_url, "button-press-event", G_CALLBACK(ftc_home_header_send_box_on_button_press_event_box_send_url), box);

    //  디자인 설정
    //  CSS 설정
    //  보내기 받기 헤더 부분 색상 설정

    //  라벨 폰트 크기 설정
    //  보내기 받기 라벨
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_send), "label_header_send_recv");

    //  버튼 이미지 설정
    gtk_image_set_from_resource(priv->imgbtn_send_clipboard, "/com/hunesion/ftcapp/COPY-ICON-16PX.png");
    gtk_image_set_from_resource(priv->imgbtn_send_url, "/com/hunesion/ftcapp/URL-ICON-16PX.png");
    gtk_image_set_from_resource(priv->imgbtn_copy_cert, "/com/hunesion/ftcapp/SEND-ICON-16PX.png");

    //  기능 활성화 여부
    if (ftc_ui_is_use_clipboard() == false) {
        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_send_clipboard),  false);
    }
    if (ftc_ui_is_use_url_redirection() == false) {
        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_send_url), false);
    }

    //  버튼 화면 배치 간격 조정
    if (gtk_widget_get_visible(GTK_WIDGET(priv->event_box_copy_cert)) == true) {
        gtk_widget_set_margin_end(GTK_WIDGET(priv->event_box_copy_cert), 35);
        margin = true;
    }

    if (gtk_widget_get_visible(GTK_WIDGET(priv->event_box_send_url)) == true) {
        if (margin) {
            gtk_widget_set_margin_end(GTK_WIDGET(priv->event_box_send_url), 10);
        } else {
            gtk_widget_set_margin_end(GTK_WIDGET(priv->event_box_send_url), 35);
            margin = true;
        }
    }

    if (gtk_widget_get_visible(GTK_WIDGET(priv->event_box_send_clipboard)) == true) {
        if (margin) {
            gtk_widget_set_margin_end(GTK_WIDGET(priv->event_box_send_clipboard), 10);
        } else {
            gtk_widget_set_margin_end(GTK_WIDGET(priv->event_box_send_clipboard), 35);
            margin = true;
        }
    }
} 

static void
ftc_home_header_send_box_dispose (GObject *object)
{
    FtcHomeHeaderSendBoxPrivate *priv;
    priv = (FtcHomeHeaderSendBoxPrivate*)ftc_home_header_send_box_get_instance_private(FTC_HOME_HEADER_SEND_BOX(object));

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);

    G_OBJECT_CLASS(ftc_home_header_send_box_parent_class)->dispose(object);
}

static void
ftc_home_header_send_box_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_home_header_send_box_parent_class)->finalize(object);
}

static void
ftc_home_header_send_box_class_init (FtcHomeHeaderSendBoxClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_header_send_box_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_header_send_box_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-header-send-box.ui");
    
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendBox, lbl_send);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendBox, event_box_send_clipboard);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendBox, event_box_send_url);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendBox, event_box_copy_cert);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendBox, imgbtn_send_clipboard);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendBox, imgbtn_send_url);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendBox, imgbtn_copy_cert);
}

FtcHomeHeaderSendBox *
ftc_home_header_send_box_new (FtcHomePage *home_page)
{
    FtcHomeHeaderSendBox *box = FTC_HOME_HEADER_SEND_BOX(g_object_new (FTC_HOME_HEADER_SEND_BOX_TYPE, NULL));
    FtcHomeHeaderSendBoxPrivate *priv = (FtcHomeHeaderSendBoxPrivate*)ftc_home_header_send_box_get_instance_private (box);

    priv->home_page = home_page;

    return box;
}

static void 
ftc_home_header_send_box_on_button_press_event_box_send_clipboard(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeHeaderSendBox *box = NULL;
    FtcHomeHeaderSendBoxPrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    
    Ftc::Core::EventManager *event_mgr = NULL;

    if (! FTC_IS_HOME_HEADER_SEND_BOX(data)) {
        return;
    }
    box = FTC_HOME_HEADER_SEND_BOX(data);

    priv = (FtcHomeHeaderSendBoxPrivate*)ftc_home_header_send_box_get_instance_private(box);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        event_mgr = Ftc::Core::EventManager::getInstance();
        if (! event_mgr) {
            return;
        }

        event_mgr->dispatchEvent(FTC_UI_SELECT_CLIPBOARD_TARGET, priv->imgbtn_send_clipboard);
    }
}

static void 
ftc_home_header_send_box_on_button_press_event_box_send_url(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeHeaderSendBox *box = NULL;
    FtcHomeHeaderSendBoxPrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;

    if (! FTC_IS_HOME_HEADER_SEND_BOX(data)) {
        return;
    }
    box = FTC_HOME_HEADER_SEND_BOX(data);

    priv = (FtcHomeHeaderSendBoxPrivate*)ftc_home_header_send_box_get_instance_private(box);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        event_mgr = Ftc::Core::EventManager::getInstance();
        if (! event_mgr) {
            return;
        }

        event_mgr->dispatchEvent(FTC_UI_SEND_URL_REDIRECTION);
    }
}