#include "ftchomeheaderrecvbox.h"


struct _FtcHomeHeaderRecvBox
{
    GtkBox parent;
};

typedef struct _FtcHomeHeaderRecvBoxPrivate FtcHomeHeaderRecvBoxPrivate;

struct _FtcHomeHeaderRecvBoxPrivate
{
    FtcHomePage *home_page;

    char        *next_page_name;

    GtkEventBox *event_box_recv_refresh;
    GtkEventBox *event_box_recv_list;
    
    GtkImage   *imgbtn_recv_refresh;
    GtkImage   *imgbtn_recv_list;
    GtkLabel    *lbl_new_recv_count;
    GtkLabel    *lbl_recv;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomeHeaderRecvBox, ftc_home_header_recv_box, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_header_recv_box_on_button_press_event_box_recv_refresh(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_home_header_recv_box_on_button_press_event_box_recv_list(GtkWidget *widget, GdkEvent *event, gpointer data);
///////////////////////////////////////////////////////////////////

static void
ftc_home_header_recv_box_init (FtcHomeHeaderRecvBox *box)
{
    FtcHomeHeaderRecvBoxPrivate *priv = NULL;
    gtk_widget_init_template (GTK_WIDGET (box));
    priv = (FtcHomeHeaderRecvBoxPrivate*)ftc_home_header_recv_box_get_instance_private (box);

    //  Todo..
    //  Signal (Event) 함수 연결
    //  버튼이 아닌 위젯이 클릭 이벤트를 받으려면 EventBox를 추가해준다.
    //  imgbtn_recv_refresh (event_box_recv_refresh) click
    g_signal_connect(priv->event_box_recv_refresh, "button-press-event", G_CALLBACK(ftc_home_header_recv_box_on_button_press_event_box_recv_refresh), box);

    //  imgbtn_recv_list (event_box_recv_list) click
    g_signal_connect(priv->event_box_recv_list, "button-press-event", G_CALLBACK(ftc_home_header_recv_box_on_button_press_event_box_recv_list), box);

    //  버튼 이미지 설정
    gtk_image_set_from_resource(priv->imgbtn_recv_refresh, "/com/hunesion/ftcapp/REFRESH-ICON-16PX.png");

    //  라벨 폰트 크기 설정
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv), "label_header_send_recv");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_new_recv_count), "label_recv_new_count");
}

static void
ftc_home_header_recv_box_dispose (GObject *object)
{
    FtcHomeHeaderRecvBoxPrivate *priv;
    priv = (FtcHomeHeaderRecvBoxPrivate*)ftc_home_header_recv_box_get_instance_private(FTC_HOME_HEADER_RECV_BOX(object));

    if (priv->next_page_name != NULL) {
        g_free(priv->next_page_name);
        priv->next_page_name = NULL;
    }

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);

    G_OBJECT_CLASS(ftc_home_header_recv_box_parent_class)->dispose(object);
}

static void
ftc_home_header_recv_box_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_home_header_recv_box_parent_class)->finalize(object);
}

static void
ftc_home_header_recv_box_class_init (FtcHomeHeaderRecvBoxClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_header_recv_box_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_header_recv_box_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-header-recv-box.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderRecvBox, event_box_recv_refresh);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderRecvBox, event_box_recv_list);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderRecvBox, imgbtn_recv_refresh);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderRecvBox, imgbtn_recv_list);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderRecvBox, lbl_new_recv_count);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeHeaderRecvBox, lbl_recv);
}

FtcHomeHeaderRecvBox *
ftc_home_header_recv_box_new (FtcHomePage *home_page, const char *next_page_name)
{
    FtcHomeHeaderRecvBox *box = FTC_HOME_HEADER_RECV_BOX(g_object_new (FTC_HOME_HEADER_RECV_BOX_TYPE, NULL));
    FtcHomeHeaderRecvBoxPrivate *priv = (FtcHomeHeaderRecvBoxPrivate*)ftc_home_header_recv_box_get_instance_private (box);
    int len = 0;

    priv->home_page = home_page;
    if (next_page_name != NULL) {
        priv->next_page_name = g_strdup(next_page_name);
    }

    //  버튼 이미지 설정
    if (strcmp(priv->next_page_name, FTC_VIEW_HOME_SUB_PAGE_DND) == 0) {
        gtk_image_set_from_resource(priv->imgbtn_recv_list, "/com/hunesion/ftcapp/ARROW-DOWN-16PX.png");
    } else if (strcmp(priv->next_page_name, FTC_VIEW_HOME_SUB_PAGE_RECV_LIST) == 0) {
        gtk_image_set_from_resource(priv->imgbtn_recv_list, "/com/hunesion/ftcapp/ARROW-UP-16PX.png");
    }

    return box;
}

static void 
ftc_home_header_recv_box_on_button_press_event_box_recv_refresh(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeHeaderRecvBox *box = NULL;
    FtcHomeHeaderRecvBoxPrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;

    if (! FTC_IS_HOME_HEADER_RECV_BOX(data)) {
        return;
    }
    box = FTC_HOME_HEADER_RECV_BOX(data);

    priv = (FtcHomeHeaderRecvBoxPrivate*)ftc_home_header_recv_box_get_instance_private(box);
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

        if (strcmp(priv->next_page_name, FTC_VIEW_HOME_SUB_PAGE_DND) == 0) {
            gpointer main_window = ftc_ui_get_main_window();
            if (main_window){
                if (ftc_ui_is_extension_widnow() == true) {
                    ftc_ui_set_extension_window(main_window, false);
                }
            }
        }

        event_mgr->dispatchEvent(FTC_CORE_RECV_LIST_REFRESH);
    }
}

static void 
ftc_home_header_recv_box_on_button_press_event_box_recv_list(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeHeaderRecvBox *box = NULL;
    FtcHomeHeaderRecvBoxPrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    gpointer main_window = NULL;
    UpdatePageEventParam update_page_evt_param;

    if (! FTC_IS_HOME_HEADER_RECV_BOX(data)) {
        return;
    }
    box = FTC_HOME_HEADER_RECV_BOX(data);

    priv = (FtcHomeHeaderRecvBoxPrivate*)ftc_home_header_recv_box_get_instance_private(box);
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
        update_page_evt_param.page_name = priv->next_page_name;

        event_mgr->dispatchEvent(FTC_UI_UPDATE_HOME_STACK_EVENT, &update_page_evt_param);

        //  세션이 만료된 상태면, next_page_name이 NULL포인트 Exception이 발생한다.
        if (! priv->next_page_name) {
            return;
        }

        if (strcmp(priv->next_page_name, FTC_VIEW_HOME_SUB_PAGE_DND) == 0) {
            main_window = ftc_ui_get_main_window();
            if (! main_window) {
                return;
            }
            update_page_evt_param.ftc_object = main_window;  /*MAIN WINDOW*/
            update_page_evt_param.page_name = FTC_VIEW_WINDOW_PAGE_RECV;

            event_mgr->dispatchEvent(FTC_UI_UPDATE_MAIN_STACK_EVENT, &update_page_evt_param);
        }
    }
}

void                    
ftc_home_header_recv_box_set_new_recv_request_count(FtcHomeHeaderRecvBox *box, int count)
{
    FtcHomeHeaderRecvBoxPrivate *priv = NULL;
    std::string str;
    
    if (! FTC_IS_HOME_HEADER_RECV_BOX(box)) {
        return;
    }
    priv = (FtcHomeHeaderRecvBoxPrivate*)ftc_home_header_recv_box_get_instance_private (box);
    if (! priv) {
        return;
    }

    str = "신규 " + std::to_string(count) + "건";
    gtk_label_set_text(priv->lbl_new_recv_count, str.c_str());
}