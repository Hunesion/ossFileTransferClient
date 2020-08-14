#include "ftcdetailrecvrequestinfosubpage.h"
#include "../core/TimeUtils.h"
#include <glib/gprintf.h>

struct _FtcDetailRecvRequestInfoSubPage
{
    GtkBox parent;
};

typedef struct _FtcDetailRecvRequestInfoSubPagePrivate FtcDetailRecvRequestInfoSubPagePrivate;

struct _FtcDetailRecvRequestInfoSubPagePrivate
{
    Ftc::Core::GlobalStruct::FtcRequest         *ftc_request;

    //  Template 에서 정의
    GtkBox      *box_info_area;
    GtkBox      *box_info_header;
    GtkBox      *box_info_body;
    
    GtkLabel    *lbl_direction;
    GtkLabel    *lbl_send_date_title;
    GtkLabel    *lbl_send_date;
    GtkLabel    *lbl_send_user_title;
    GtkLabel    *lbl_send_user;
    GtkLabel    *lbl_recv_user_title;
    GtkLabel    *lbl_recv_user;
    GtkLabel    *lbl_expired_date_title;
    GtkLabel    *lbl_expired_date;
    GtkLabel    *lbl_approval_status_title;
    GtkLabel    *lbl_approval_status;
    GtkEventBox *event_box_approval_status;
    GtkImage    *imgbtn_approval_status;
    GtkLabel    *lbl_send_reason_title;
    GtkTextView *txtview_send_reason;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcDetailRecvRequestInfoSubPage, ftc_detail_recv_request_info_sub_page, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_detail_recv_request_info_sub_page_on_button_press_event_box_approval_status(GtkWidget *widget, GdkEvent *event, gpointer data);
///////////////////////////////////////////////////////////////////

static void
ftc_detail_recv_request_info_sub_page_init (FtcDetailRecvRequestInfoSubPage *page)
{
    FtcDetailRecvRequestInfoSubPagePrivate *priv = NULL;

    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcDetailRecvRequestInfoSubPagePrivate*)ftc_detail_recv_request_info_sub_page_get_instance_private (page);

    //  Todo..

    //  Signal (Event) 함수 연결
    //  ckbtn_all_check toggle active
    g_signal_connect(priv->event_box_approval_status, "button-press-event", G_CALLBACK(ftc_detail_recv_request_info_sub_page_on_button_press_event_box_approval_status), page);

    //  Widget 크기 설정
    //
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_info_area), 400, 520);
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_info_header), 400, 30);
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_info_body), 400, 490);

    //  이미지 설정
    //
    gtk_image_set_from_resource(priv->imgbtn_approval_status, "/com/hunesion/ftcapp/APPROVAL-LIST-ICON-16PX.png");

    //  CSS 설정
    //  Background 색상 설정
    gtk_widget_set_name(GTK_WIDGET(priv->box_info_header), "box_detail_header_sub_title");
    gtk_widget_set_name(GTK_WIDGET(priv->box_info_body), "default_background_page");

    //  위젯설정
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_direction), "label_send_detail_sub_title");

    gtk_widget_set_name(GTK_WIDGET(priv->lbl_send_date_title), "label_recv_info_title_bold");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_send_user_title), "label_recv_info_title_bold");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_user_title), "label_recv_info_title_bold");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_expired_date_title), "label_recv_info_title_bold");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_approval_status_title), "label_recv_info_title_bold");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_send_reason_title), "label_recv_info_title_bold");

    gtk_widget_set_name(GTK_WIDGET(priv->lbl_send_date), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_send_user), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_user), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_expired_date), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_approval_status), "label_recv_info_title");

    gtk_text_view_set_editable(priv->txtview_send_reason, FALSE);
    gtk_text_view_set_wrap_mode(priv->txtview_send_reason, GTK_WRAP_WORD_CHAR);
}

static void
ftc_detail_recv_request_info_sub_page_dispose (GObject *object)
{
    FtcDetailRecvRequestInfoSubPagePrivate *priv = NULL;
    priv = (FtcDetailRecvRequestInfoSubPagePrivate*)ftc_detail_recv_request_info_sub_page_get_instance_private(FTC_DETAIL_RECV_REQUEST_INFO_SUB_PAGE(object));

    priv->ftc_request = NULL;

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);
    
    G_OBJECT_CLASS(ftc_detail_recv_request_info_sub_page_parent_class)->dispose(object);
}

static void
ftc_detail_recv_request_info_sub_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_detail_recv_request_info_sub_page_parent_class)->finalize(object);
}

static void
ftc_detail_recv_request_info_sub_page_class_init (FtcDetailRecvRequestInfoSubPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_detail_recv_request_info_sub_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_detail_recv_request_info_sub_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-detail-recv-request-info-sub-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, box_info_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, box_info_header);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, box_info_body);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_direction);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_send_date_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_send_date);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_send_user_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_send_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_recv_user_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_recv_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_expired_date_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_expired_date);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_approval_status_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_approval_status);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, event_box_approval_status);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, imgbtn_approval_status);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, lbl_send_reason_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvRequestInfoSubPage, txtview_send_reason);
}

FtcDetailRecvRequestInfoSubPage *
ftc_detail_recv_request_info_sub_page_new ()
{
    FtcDetailRecvRequestInfoSubPage *page = FTC_DETAIL_RECV_REQUEST_INFO_SUB_PAGE(g_object_new (FTC_DETAIL_RECV_REQUEST_INFO_SUB_PAGE_TYPE, NULL));
    FtcDetailRecvRequestInfoSubPagePrivate *priv = (FtcDetailRecvRequestInfoSubPagePrivate*)ftc_detail_recv_request_info_sub_page_get_instance_private (page);

    return page;
}


void                    
ftc_detail_recv_request_info_sub_page_set_data(FtcDetailRecvRequestInfoSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request)
{
    FtcDetailRecvRequestInfoSubPagePrivate *priv = NULL;
    std::string str = "";
    GtkTextBuffer *txt_buf = NULL;
    char datetime[20] = { 0, };
    int seconds = 0, expired_days = 0;
    std::time_t tmp_date, now_date;
    std::tm now_tm = Ftc::Core::TimeUtils::now(), tmp_tm;

    if (! FTC_IS_DETAIL_RECV_REQUEST_INFO_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailRecvRequestInfoSubPagePrivate*)ftc_detail_recv_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! ftc_request) {
        return;
    }

    priv->ftc_request = ftc_request;

    //  보내는망 > 받는망
    str = ftc_request->from_network_name + "  >  " + ftc_request->to_network_name;
    gtk_label_set_text(priv->lbl_direction, str.c_str());

    //  보낸날짜
    g_snprintf(datetime, sizeof(datetime), "%s-%s-%s %s:%s:%s", 
            ftc_request->request_dt.substr(0, 4).c_str(), ftc_request->request_dt.substr(4, 2).c_str(), ftc_request->request_dt.substr(6, 2).c_str(),
            ftc_request->request_dt.substr(8, 2).c_str(), ftc_request->request_dt.substr(10, 2).c_str(), ftc_request->request_dt.substr(12, 2).c_str());
    gtk_label_set_text(priv->lbl_send_date, datetime);
    memset(datetime, 0, sizeof(datetime));
    //  보낸사람
    str = ftc_request->ftc_request_info.request_user_name + "(" + ftc_request->ftc_request_info.request_user_id + "/" + ftc_request->ftc_request_info.request_user_group_name + ")";
    gtk_label_set_text(priv->lbl_send_user, str.c_str());
    //  받는사람
    str = ftc_request->ftc_request_info.receive_name + "(" + ftc_request->ftc_request_info.receive_id + "/" + ftc_request->ftc_request_info.receive_user_group_name + ")";
    gtk_label_set_text(priv->lbl_recv_user, str.c_str());
    //  유효기간
    if (ftc_request->download_expire_dt.length() > 0) {
        g_snprintf(datetime, sizeof(datetime), "%s-%s-%s",
                ftc_request->download_expire_dt.substr(0, 4).c_str(), ftc_request->download_expire_dt.substr(4, 2).c_str(), ftc_request->download_expire_dt.substr(6, 2).c_str());

        if (seconds > 0) {
            //  유효기간이 남아있음
            expired_days = seconds / 60 / 60 / 24;
            str = std::to_string(expired_days) + "일 남음(~" + std::string(datetime) + ")";
        } else {
            str = "기간 만료(~" + std::string(datetime) + ")";
        }
    } else {
        str = "없음";
    }
    
    gtk_label_set_text(priv->lbl_expired_date, str.c_str());
    //  결재상태
    str = ftc_ui_get_approval_state_text(ftc_request->ftc_request_approval_info.approval_state, ftc_request->ftc_request_approval_info.approval_type, ftc_request->ftc_request_approval_info.last_approval_type);
    gtk_label_set_text(priv->lbl_approval_status, str.c_str());

    if (str == "결재예외") {
        //  결재예외일 경우 결재선 웹뷰 버튼을 하이드 시킨다.
        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_approval_status), false);
    } else {
        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_approval_status), true);
    }

    //  전송사유
    txt_buf = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(txt_buf, ftc_request->ftc_request_info.request_text.c_str(), ftc_request->ftc_request_info.request_text.length());
    gtk_text_view_set_buffer(priv->txtview_send_reason, txt_buf);
}


static void 
ftc_detail_recv_request_info_sub_page_on_button_press_event_box_approval_status(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcDetailRecvRequestInfoSubPage *page = NULL;
    FtcDetailRecvRequestInfoSubPagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    gpointer main_window = NULL;

    if (! FTC_IS_DETAIL_RECV_REQUEST_INFO_SUB_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_RECV_REQUEST_INFO_SUB_PAGE(data);

    priv = (FtcDetailRecvRequestInfoSubPagePrivate*)ftc_detail_recv_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "SELECT_PACK_FILE_NETWORK", NULL, "결재선", 800, 120); 
    }
}