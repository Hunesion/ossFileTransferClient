#include "ftchomeheadersendhistorybox.h"


struct _FtcHomeHeaderSendHistoryBox
{
    GtkBox parent;
};

typedef struct _FtcHomeHeaderSendHistoryBoxPrivate FtcHomeHeaderSendHistoryBoxPrivate;

struct _FtcHomeHeaderSendHistoryBoxPrivate
{
    FtcHomePage *home_page;

    GtkEventBox *event_box_send_refresh;
    GtkEventBox *event_box_referer;
    GtkImage    *imgbtn_referer;
    GtkLabel    *lbl_send_history;
    GtkImage    *imgbtn_send_refresh;
    GtkLabel    *lbl_new_send_count;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomeHeaderSendHistoryBox, ftc_home_header_send_history_box, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_header_send_history_box_on_button_press_event_box_history(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_home_header_send_history_box_on_button_press_event_send_refresh(GtkWidget *widget, GdkEvent *event, gpointer data);
///////////////////////////////////////////////////////////////////

static void
ftc_home_header_send_history_box_init (FtcHomeHeaderSendHistoryBox *box)
{
    FtcHomeHeaderSendHistoryBoxPrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = Ftc::Core::EventManager::getInstance();
    gtk_widget_init_template (GTK_WIDGET (box));
    priv = (FtcHomeHeaderSendHistoryBoxPrivate*)ftc_home_header_send_history_box_get_instance_private (box);

    //  Todo..
    //  Signal (Event) 함수 연결
    //  버튼이 아닌 위젯이 클릭 이벤트를 받으려면 EventBox를 추가해준다.
    //  imgbtn_send_history (event_box_send_history) click
    g_signal_connect(priv->event_box_referer, "button-press-event", G_CALLBACK(ftc_home_header_send_history_box_on_button_press_event_box_history), box);

    //  imgbtn_send_refresh (event_box_send_refresh) click
    g_signal_connect(priv->event_box_send_refresh, "button-press-event", G_CALLBACK(ftc_home_header_send_history_box_on_button_press_event_send_refresh), box);

    //  버튼 이미지 설정
    gtk_image_set_from_resource(priv->imgbtn_referer, "/com/hunesion/ftcapp/ARROW-DOWN-16PX.png");
    gtk_image_set_from_resource(priv->imgbtn_send_refresh, "/com/hunesion/ftcapp/REFRESH-ICON-16PX.png");

    //  라벨 폰트 크기 설정
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_send_history), "label_header_send_recv");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_new_send_count), "label_recv_new_count");
}

static void
ftc_home_header_send_history_box_dispose (GObject *object)
{
    FtcHomeHeaderSendHistoryBoxPrivate *priv;
    priv = (FtcHomeHeaderSendHistoryBoxPrivate*)ftc_home_header_send_history_box_get_instance_private(FTC_HOME_HEADER_SEND_HISTORY_BOX(object));

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);

    G_OBJECT_CLASS(ftc_home_header_send_history_box_parent_class)->dispose(object);
}

static void
ftc_home_header_send_history_box_finalize(GObject *object)
{

    G_OBJECT_CLASS(ftc_home_header_send_history_box_parent_class)->finalize(object);
}

static void
ftc_home_header_send_history_box_class_init (FtcHomeHeaderSendHistoryBoxClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_header_send_history_box_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_header_send_history_box_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-header-send-history-box.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendHistoryBox, event_box_referer);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendHistoryBox, imgbtn_referer);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendHistoryBox, lbl_send_history);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendHistoryBox, event_box_send_refresh);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendHistoryBox, imgbtn_send_refresh);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderSendHistoryBox, lbl_new_send_count);
}

FtcHomeHeaderSendHistoryBox *
ftc_home_header_send_history_box_new (FtcHomePage *home_page)
{
    FtcHomeHeaderSendHistoryBox *box = FTC_HOME_HEADER_SEND_HISTORY_BOX(g_object_new (FTC_HOME_HEADER_SEND_HISTORY_BOX_TYPE, NULL));
    FtcHomeHeaderSendHistoryBoxPrivate *priv = (FtcHomeHeaderSendHistoryBoxPrivate*)ftc_home_header_send_history_box_get_instance_private (box);
    priv->home_page = home_page;

    return box;
}

static void 
ftc_home_header_send_history_box_on_button_press_event_box_history(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeHeaderSendHistoryBox *box = NULL;
    FtcHomeHeaderSendHistoryBoxPrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    gpointer main_window = NULL;
    UpdatePageEventParam update_page_evt_param;

    if (! FTC_IS_HOME_HEADER_SEND_HISTORY_BOX(data)) {
        return;
    }
    box = FTC_HOME_HEADER_SEND_HISTORY_BOX(data);

    priv = (FtcHomeHeaderSendHistoryBoxPrivate*)ftc_home_header_send_history_box_get_instance_private(box);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/) {

        event_mgr = Ftc::Core::EventManager::getInstance();
        if (! event_mgr) {
            return;
        }

        update_page_evt_param.ftc_object = priv->home_page;
        update_page_evt_param.page_name = FTC_VIEW_HOME_SUB_PAGE_DND;

        event_mgr->dispatchEvent(FTC_UI_UPDATE_HOME_STACK_EVENT, &update_page_evt_param);

        main_window = ftc_ui_get_main_window();
        if (! main_window) {
            return;
        }
        update_page_evt_param.ftc_object = main_window;  /*MAIN WINDOW*/
        update_page_evt_param.page_name = FTC_VIEW_WINDOW_PAGE_RECV;

        event_mgr->dispatchEvent(FTC_UI_UPDATE_MAIN_STACK_EVENT, &update_page_evt_param);
        
    }
}

static void 
ftc_home_header_send_history_box_on_button_press_event_send_refresh(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeHeaderSendHistoryBox *box = NULL;
    FtcHomeHeaderSendHistoryBoxPrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;

    if (! FTC_IS_HOME_HEADER_SEND_HISTORY_BOX(data)) {
        return;
    }
    box = FTC_HOME_HEADER_SEND_HISTORY_BOX(data);

    priv = (FtcHomeHeaderSendHistoryBoxPrivate*)ftc_home_header_send_history_box_get_instance_private(box);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/) {

        event_mgr = Ftc::Core::EventManager::getInstance();
        if (! event_mgr) {
            return;
        }

        gpointer main_window = ftc_ui_get_main_window();
        if (main_window){
            if (ftc_ui_is_extension_widnow() == true) {
                ftc_ui_set_extension_window(main_window, false);
            }
        }

        event_mgr->dispatchEvent(FTC_UI_SEND_HISTORY_REFRESH, priv->home_page);
    }
}

void                            
ftc_home_header_send_history_set_new_count(FtcHomeHeaderSendHistoryBox *box, int new_count)
{
    FtcHomeHeaderSendHistoryBoxPrivate *priv = NULL;
    std::string str;

    if (! FTC_IS_HOME_HEADER_SEND_HISTORY_BOX(box)) {
        return;
    }

    priv = (FtcHomeHeaderSendHistoryBoxPrivate*)ftc_home_header_send_history_box_get_instance_private(box);
    if(! priv) {
        return;
    }

    str = "신규 " + std::to_string(new_count) + "건";

    gtk_label_set_text(priv->lbl_new_send_count, str.c_str());
}