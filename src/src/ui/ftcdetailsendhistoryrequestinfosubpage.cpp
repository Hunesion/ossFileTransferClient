#include "ftcdetailsendhistoryrequestinfosubpage.h"
#include "../core/StringUtils.h"
#include <glib/gprintf.h>

typedef struct _FindRequestInfoParam
{
    gpointer find_data;
    RequestInfo *request_info;

    _FindRequestInfoParam() {
        find_data = NULL;
        request_info = NULL;
    }
} FindRequestInfoParam;

struct _FtcDetailSendHistoryRequestInfoSubPage
{
    GtkBox parent;
};

typedef struct _FtcDetailSendHistoryRequestInfoSubPagePrivate FtcDetailSendHistoryRequestInfoSubPagePrivate;

struct _FtcDetailSendHistoryRequestInfoSubPagePrivate
{
    Ftc::Core::GlobalStruct::FtcRequest         *ftc_request;
    GSList                      *request_info_list;
    RequestInfo                 *selection_request_info;
    FtcDetailSendHistoryPage    *sendhistory_page;

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

G_DEFINE_TYPE_WITH_PRIVATE(FtcDetailSendHistoryRequestInfoSubPage, ftc_detail_send_history_request_info_sub_page, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_detail_send_history_request_info_sub_page_on_click_target_network(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_send_history_request_info_sub_page_update_target_network_screen(FtcDetailSendHistoryRequestInfoSubPage *page);

static void ftc_detail_send_history_request_info_sub_page_set_target_network_list(FtcDetailSendHistoryRequestInfoSubPage *page);

static void ftc_detail_send_history_request_info_sub_page_set_target_network_list_callback(gpointer data, gpointer user_data);

static void ftc_detail_send_history_request_info_sub_page_clear_target_network_list(FtcDetailSendHistoryRequestInfoSubPage *page);

static void ftc_detail_send_history_request_info_sub_page_clear_target_network_list_callback(GtkWidget *widget, gpointer user_data);

static RequestInfo* ftc_detail_send_history_request_info_sub_page_find_request_info_for_target_network_widget(FtcDetailSendHistoryRequestInfoSubPage *page, GtkWidget *widget);

static void ftc_detail_send_history_request_info_sub_page_find_request_info_for_target_network_widget_callback(gpointer data, gpointer user_data);

static RequestInfo* ftc_detail_send_history_request_info_sub_page_find_request_info_for_to_network_id(FtcDetailSendHistoryRequestInfoSubPage *page, std::string &to_network_id);

static void ftc_detail_send_history_request_info_sub_page_find_request_info_for_to_network_id_callback(gpointer data, gpointer user_data);

static void ftc_detail_send_history_request_info_sub_page_update_target_network_button_activate_color(FtcDetailSendHistoryRequestInfoSubPage *page);

static void ftc_detail_send_history_request_info_sub_page_update_target_network_button_activate_color_callback(GtkWidget *widget, gpointer user_data);

static void ftc_detail_send_history_request_info_sub_page_on_button_press_event_box_approval_status(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_send_history_request_info_sub_page_update_approval_screen(FtcDetailSendHistoryRequestInfoSubPage *page);
///////////////////////////////////////////////////////////////////

static void
ftc_detail_send_history_request_info_sub_page_init (FtcDetailSendHistoryRequestInfoSubPage *page)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    std::string str;

    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private (page);

    //  Signal (Event) 함수 연결
    //  ckbtn_all_check toggle active
    g_signal_connect(priv->event_box_approval_status, "button-press-event", G_CALLBACK(ftc_detail_send_history_request_info_sub_page_on_button_press_event_box_approval_status), page);

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
ftc_detail_send_history_request_info_sub_page_dispose (GObject *object)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(object));

    priv->ftc_request = NULL;
    priv->request_info_list = NULL;
    priv->selection_request_info = NULL;

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);
    
    G_OBJECT_CLASS(ftc_detail_send_history_request_info_sub_page_parent_class)->dispose(object);
}

static void
ftc_detail_send_history_request_info_sub_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_detail_send_history_request_info_sub_page_parent_class)->finalize(object);
}

static void
ftc_detail_send_history_request_info_sub_page_class_init (FtcDetailSendHistoryRequestInfoSubPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_detail_send_history_request_info_sub_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_detail_send_history_request_info_sub_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-detail-send-history-request-info-sub-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, box_info_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, box_info_header);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, box_info_body);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_direction);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_send_date_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_send_date);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_send_user_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_send_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_recv_user_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_recv_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_expired_date_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_expired_date);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_approval_status_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_approval_status);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, event_box_approval_status);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, imgbtn_approval_status);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, lbl_send_reason_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryRequestInfoSubPage, txtview_send_reason);
}

FtcDetailSendHistoryRequestInfoSubPage *
ftc_detail_send_history_request_info_sub_page_new (FtcDetailSendHistoryPage *sendhistory_page)
{
    FtcDetailSendHistoryRequestInfoSubPage *page = FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(g_object_new (FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE_TYPE, NULL));
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private (page);
    if (! priv) {
        return page;
    }
    priv->sendhistory_page = sendhistory_page;

    return page;
}

void                    
ftc_detail_send_history_request_info_sub_page_set_data(FtcDetailSendHistoryRequestInfoSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, GSList *request_info_list)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    std::string str = "";
    GtkTextBuffer *txt_buf = NULL;
    char datetime[20] = { 0, };
    std::time_t now_date = std::time(NULL);
    std::tm now_tm;

    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! ftc_request) {
        return;
    }

    priv->ftc_request = ftc_request;
    priv->request_info_list = request_info_list;

    //  대상망 구성
    ftc_detail_send_history_request_info_sub_page_set_target_network_list(page);

    //  보낸날짜
    localtime_r(&now_date, &now_tm);
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

    //  전송사유
    txt_buf = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(txt_buf, ftc_request->ftc_request_info.request_text.c_str(), ftc_request->ftc_request_info.request_text.length());
    gtk_text_view_set_buffer(priv->txtview_send_reason, txt_buf);

    //  초기 데이터는 첫번째 망 데이터를 입력한다.
    priv->selection_request_info = (RequestInfo*)g_slist_nth_data(priv->request_info_list, 1);

    //  망 선택에 따른 화면 구성
    ftc_detail_send_history_request_info_sub_page_update_target_network_screen(page);
}

static void
ftc_detail_send_history_request_info_sub_page_update_target_network_screen(FtcDetailSendHistoryRequestInfoSubPage *page)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    std::string str = "";
    char datetime[20] = { 0, };
    std::time_t now_date = std::time(NULL), tmp_date, calc_date;
    std::tm now_tm, tmp_tm;
    int seconds = 0, expired_days = 0;

    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! priv->selection_request_info) {
        return;
    }

    ftc_detail_send_history_request_info_sub_page_update_target_network_button_activate_color(page);

    //  유효기간
    if (priv->selection_request_info->download_expire_dt.length() > 0) {
        localtime_r(&now_date, &now_tm);

        tmp_tm.tm_year = atoi(priv->selection_request_info->download_expire_dt.substr(0, 4).c_str()) - 1900;
        tmp_tm.tm_mon = atoi(priv->selection_request_info->download_expire_dt.substr(4, 2).c_str()) - 1;
        tmp_tm.tm_mday = atoi(priv->selection_request_info->download_expire_dt.substr(6, 2).c_str());
        //  시간 계산 시 초단위 까지 계산함으로 유효 날짜(일 단위)로 표기하기 위해 현재 시분초를 넣어준다.
        tmp_tm.tm_hour = now_tm.tm_hour;
        tmp_tm.tm_min = now_tm.tm_min;
        tmp_tm.tm_sec = now_tm.tm_sec;
        
        tmp_date = std::mktime(&tmp_tm);
        seconds = std::difftime(tmp_date, now_date);

        g_snprintf(datetime, sizeof(datetime), "%s-%s-%s",
                priv->selection_request_info->download_expire_dt.substr(0, 4).c_str(), priv->selection_request_info->download_expire_dt.substr(4, 2).c_str(), priv->selection_request_info->download_expire_dt.substr(6, 2).c_str());
        if (seconds > 0) {
            //  유효기간이 남아있음
            expired_days = seconds / 60 / 60 / 24;

            Ftc::Core::StringUtils::format(str, "%d일 남음(~%s)", expired_days, datetime);
        } else {
            //  기간 만료(~%s)
            Ftc::Core::StringUtils::format(str, "기간 만료(~%s)", datetime);
        }
    } else {
        str = "없음";   
    }
    gtk_label_set_text(priv->lbl_expired_date, str.c_str());

    //  결재상태
    str = ftc_ui_get_approval_state_text(priv->selection_request_info->approval.approval_state, priv->selection_request_info->approval.approval_type, priv->selection_request_info->approval.last_approval_type);
    gtk_label_set_text(priv->lbl_approval_status, str.c_str());

    if (str == "결재예외") {
        //  결재예외일 경우 결재선 웹뷰 버튼을 하이드 시킨다.
        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_approval_status), false);
    } else {
        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_approval_status), true);
    }
}

static void
ftc_detail_send_history_request_info_sub_page_on_click_target_network(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcDetailSendHistoryRequestInfoSubPage *page = NULL;
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    GdkEventButton *event_button = (GdkEventButton*)event;
    
    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(data);

    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        priv->selection_request_info = ftc_detail_send_history_request_info_sub_page_find_request_info_for_target_network_widget(page, widget);
        ftc_detail_send_history_request_info_sub_page_update_target_network_screen(page);
    }
}

static void
ftc_detail_send_history_request_info_sub_page_set_target_network_list(FtcDetailSendHistoryRequestInfoSubPage *page)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    
    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! priv->request_info_list) {
        return;
    }
    ftc_detail_send_history_request_info_sub_page_clear_target_network_list(page);
    g_slist_foreach(priv->request_info_list, ftc_detail_send_history_request_info_sub_page_set_target_network_list_callback, page);
}

static void
ftc_detail_send_history_request_info_sub_page_set_target_network_list_callback(gpointer data, gpointer user_data)
{
    FtcDetailSendHistoryRequestInfoSubPage *page = NULL;
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    RequestInfo *request_info = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(user_data)) {
        return;
    }
    page = FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(user_data);

    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! data) {
        return;
    }

    request_info = (RequestInfo*)data;
    request_info->event_box = gtk_event_box_new();
    g_signal_connect(request_info->event_box, "button-press-event", G_CALLBACK(ftc_detail_send_history_request_info_sub_page_on_click_target_network), page);
    gtk_widget_set_margin_end(request_info->event_box, 8);

    request_info->widget_target_network = gtk_label_new(request_info->network.to_network_name.c_str());
    gtk_container_add(GTK_CONTAINER(request_info->event_box), request_info->widget_target_network);
    
    gtk_widget_set_name(request_info->widget_target_network, "label_send_detail_sub_title");
    gtk_widget_show_all(request_info->event_box);
    
    gtk_box_pack_start(priv->box_info_header, request_info->event_box, false, false, 0);
}

static void
ftc_detail_send_history_request_info_sub_page_clear_target_network_list(FtcDetailSendHistoryRequestInfoSubPage *page)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    
    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->box_info_header), ftc_detail_send_history_request_info_sub_page_clear_target_network_list_callback, page);
}

static void 
ftc_detail_send_history_request_info_sub_page_clear_target_network_list_callback(GtkWidget *widget, gpointer user_data)
{
    FtcDetailSendHistoryRequestInfoSubPage *page = NULL;
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(user_data)) {
        return;
    }
    page = FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(user_data);

    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (GTK_IS_EVENT_BOX(widget)) {
        gtk_container_remove(GTK_CONTAINER(priv->box_info_header), widget);
    }
}

static RequestInfo* 
ftc_detail_send_history_request_info_sub_page_find_request_info_for_target_network_widget(FtcDetailSendHistoryRequestInfoSubPage *page, GtkWidget *widget)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    FindRequestInfoParam param;
    RequestInfo *rv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    param.find_data = widget;
    g_slist_foreach(priv->request_info_list, ftc_detail_send_history_request_info_sub_page_find_request_info_for_target_network_widget_callback, &param);

    rv = param.request_info;
    return rv;
}

static void 
ftc_detail_send_history_request_info_sub_page_find_request_info_for_target_network_widget_callback(gpointer data, gpointer user_data)
{
    FindRequestInfoParam *param = NULL;
    RequestInfo *request_info = NULL;

    if (! data || ! user_data) {
        return;
    }

    param = (FindRequestInfoParam*)user_data;
    request_info = (RequestInfo*)data;
    
    if (request_info->event_box == param->find_data) {
        param->request_info = request_info;
    }
}

static RequestInfo*
ftc_detail_send_history_request_info_sub_page_find_request_info_for_to_network_id(FtcDetailSendHistoryRequestInfoSubPage *page, std::string &to_network_id)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    FindRequestInfoParam param;
    RequestInfo *rv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    param.find_data = (gpointer)to_network_id.c_str();
    g_slist_foreach(priv->request_info_list, ftc_detail_send_history_request_info_sub_page_find_request_info_for_to_network_id_callback, &param);

    rv = param.request_info;
    return rv;
}

static void 
ftc_detail_send_history_request_info_sub_page_find_request_info_for_to_network_id_callback(gpointer data, gpointer user_data)
{
    FindRequestInfoParam *param = NULL;
    RequestInfo *request_info = NULL;

    if (! data || ! user_data) {
        return;
    }

    param = (FindRequestInfoParam*)user_data;
    request_info = (RequestInfo*)data;
    
    if (request_info->network.to_network_id == (char*)param->find_data) {
        param->request_info = request_info;
    }
}

static void
ftc_detail_send_history_request_info_sub_page_update_target_network_button_activate_color(FtcDetailSendHistoryRequestInfoSubPage *page)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    
    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->box_info_header), 
                        ftc_detail_send_history_request_info_sub_page_update_target_network_button_activate_color_callback, page);
}

static void
ftc_detail_send_history_request_info_sub_page_update_target_network_button_activate_color_callback(GtkWidget *widget, gpointer user_data)
{
    FtcDetailSendHistoryRequestInfoSubPage *page = NULL;
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    RequestInfo *request_info = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(user_data)) {
        return;
    }
    page = FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(user_data);

    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (GTK_IS_EVENT_BOX(widget)) {
        if (priv->selection_request_info->event_box == widget) {
            gtk_widget_set_name(priv->selection_request_info->widget_target_network, "label_send_detail_sub_title_select");
        } else {
            request_info = ftc_detail_send_history_request_info_sub_page_find_request_info_for_target_network_widget(page, widget);
            gtk_widget_set_name(request_info->widget_target_network, "label_send_detail_sub_title");
        }
    }
}

static void 
ftc_detail_send_history_request_info_sub_page_on_button_press_event_box_approval_status(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcDetailSendHistoryRequestInfoSubPage *page = NULL;
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    gpointer main_window = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(data);

    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! priv->selection_request_info) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        main_window = ftc_ui_get_main_window();
        if (! main_window) {
            return;
        }

        //  결재선
        ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "WEB_KIT_URL_SELECT_PACK_FILE_NETWORK", NULL, 
                                    "결재선", 800, 120, NULL, NULL, true, false, false); 

        ftc_detail_send_history_request_info_sub_page_update_approval_screen(page);
    }
}

void
ftc_detail_send_history_request_info_sub_page_update_approval_screen(FtcDetailSendHistoryRequestInfoSubPage *page)
{
    FtcDetailSendHistoryRequestInfoSubPagePrivate *priv = NULL;
    std::string selection_network_id;
    
    if (! FTC_IS_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendHistoryRequestInfoSubPagePrivate*)ftc_detail_send_history_request_info_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    //  결재정보를 업데이트 하면 request_info_list가 초기화 후 다시 생성된다.
    //  초기화 되기 전 현재 선택중인 네트워크의 id를 백업한다.
    selection_network_id = priv->selection_request_info->network.to_network_id;

    priv->request_info_list = ftc_detail_send_history_page_update_approval_info(priv->sendhistory_page);

    //  대상망 구성
    ftc_detail_send_history_request_info_sub_page_set_target_network_list(page);   

    priv->selection_request_info = ftc_detail_send_history_request_info_sub_page_find_request_info_for_to_network_id(page, selection_network_id);
    ftc_detail_send_history_request_info_sub_page_update_target_network_screen(page);   
}