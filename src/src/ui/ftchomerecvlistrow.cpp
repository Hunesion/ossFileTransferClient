#include "ftchomerecvlistrow.h"
#include "ftcmainwindow.h"
#include "../core/StringUtils.h"


struct _FtcHomeRecvListRow
{
    GtkBox parent;
};

typedef struct _FtcHomeRecvListRowPrivate FtcHomeRecvListRowPrivate;

struct _FtcHomeRecvListRowPrivate
{
    FtcHomeRecvListSubPage *recvlist_sub_page;

    GtkEventBox *event_box_batch_download;
    GtkImage    *imgbtn_batch_download;
    GtkLabel    *lbl_recv_title;
    GtkLabel    *lbl_recv_summary;
    GtkBox      *box_progress_area;
    GtkLabel    *lbl_progress;
    GtkProgressBar *pbar_progress;
    

    RequestDownloadState* download_state;

    Ftc::Core::GlobalStruct::FtcRequest *ftc_request;
    bool is_expired_date;
    int total_count;
    int count;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomeRecvListRow, ftc_home_recv_list_row, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_recv_list_row_on_button_press_event_box_batch_download(GtkWidget *widget, GdkEvent *event, gpointer data);
///////////////////////////////////////////////////////////////////

static void
ftc_home_recv_list_row_init (FtcHomeRecvListRow *row)
{
    FtcHomeRecvListRowPrivate *priv = NULL;
    gtk_widget_init_template (GTK_WIDGET (row));
    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private (row);

    //  Todo..
    //  Signal (Event) 함수 연결
    //  imgbtn_batch_download(event_box_batch_download) click
    g_signal_connect(priv->event_box_batch_download, "button-press-event", G_CALLBACK(ftc_home_recv_list_row_on_button_press_event_box_batch_download), row);

    //  버튼 이미지 적용
    //
    if (ftc_ui_is_internal_location()) {
        // 업무망
        gtk_image_set_from_resource(priv->imgbtn_batch_download, "/com/hunesion/ftcapp/DOWNLOAD-ICON-IS.png");
    } else {
        // 인터넷망
        gtk_image_set_from_resource(priv->imgbtn_batch_download, "/com/hunesion/ftcapp/DOWNLOAD-ICON-ES.png");
    }

    //  Widget CSS 적용
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_progress), "label_send_progress");
    gtk_widget_set_name(GTK_WIDGET(priv->pbar_progress), "progressbar_individual_upload");
    if (ftc_ui_is_internal_location()) {
        //  업무 망 
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_summary), "label_recv_info_contents_is");
    } else {
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_summary), "label_recv_info_contents_es");
    }
    

}

static void
ftc_home_recv_list_row_dispose (GObject *object)
{
    FtcHomeRecvListRowPrivate *priv;
    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(FTC_HOME_RECV_LIST_ROW(object));

    if (priv->ftc_request) {
        delete priv->ftc_request;
        priv->ftc_request = NULL;
    }

    if (priv->download_state) {
        delete priv->download_state;
        priv->download_state = NULL;
    }

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);
 
    G_OBJECT_CLASS(ftc_home_recv_list_row_parent_class)->dispose(object);
}

static void
ftc_home_recv_list_row_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_home_recv_list_row_parent_class)->finalize(object);
}

static void
ftc_home_recv_list_row_class_init (FtcHomeRecvListRowClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_recv_list_row_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_recv_list_row_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-recv-list-row.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeRecvListRow, event_box_batch_download);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeRecvListRow, imgbtn_batch_download);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeRecvListRow, lbl_recv_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeRecvListRow, lbl_recv_summary);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeRecvListRow, box_progress_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeRecvListRow, lbl_progress);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeRecvListRow, pbar_progress);
}

FtcHomeRecvListRow *
ftc_home_recv_list_row_new (FtcHomeRecvListSubPage *recvlist_sub_page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request)
{
    FtcHomeRecvListRow *row = FTC_HOME_RECV_LIST_ROW(g_object_new (FTC_HOME_RECV_LIST_ROW_TYPE, NULL));
    FtcHomeRecvListRowPrivate *priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    GtkWidget  *lbl_reduce = NULL;
    std::string title_show, str;

    priv->recvlist_sub_page = recvlist_sub_page;
    priv->ftc_request = ftc_request;

    priv->download_state = new RequestDownloadState();

    //  픽셀계산(ftc_ui_reduce_text_pixel) 후 CSS 변경(gtk_widget_set_name) 시 동작하지 않는 버그가 있어
    //  픽셀계산용 label을 생성하여 계산한다.
    lbl_reduce = gtk_label_new("");
    
    //  읽지않은 파일일 경우 제목을 BOLD(굵게)처리 한다.
    if (priv->ftc_request->is_read == false) {
        gtk_widget_set_name(lbl_reduce, "label_recv_info_title_bold"); 
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_title), "label_recv_info_title_bold"); 
    } else {
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_title), "label_recv_info_title");
        gtk_widget_set_name(lbl_reduce, "label_recv_info_title"); 
    }

    //  제목
    title_show = ftc_ui_reduce_text_pixel(lbl_reduce, ftc_request->request_title.c_str(), 300);
    gtk_label_set_text(priv->lbl_recv_title, title_show.c_str());
    if (title_show != ftc_request->request_title) {
        gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_recv_title), ftc_request->request_title.c_str());
    }
    
    if (priv->ftc_request->file_cnt > 0) {
        //  요약
        str = priv->ftc_request->firstFileName;
        if (priv->ftc_request->file_cnt > 1) {
            str += " 외 " + std::to_string(priv->ftc_request->file_cnt - 1) + "개";
        }
        str += ", " + ftc_ui_get_file_size_format(priv->ftc_request->totalFileSize);
        gtk_label_set_text(priv->lbl_recv_summary, str.c_str());
    } else {
        gtk_label_set_text(priv->lbl_recv_summary, ftc_ui_get_file_size_format(priv->ftc_request->totalFileSize).c_str());
    }
    
    //  기간 만료 시 다운로드 버튼 안보이게
    if (ftc_ui_is_expired_date_request_info(priv->ftc_request->download_expire_dt) || priv->ftc_request->file_cnt <= 0) {
        priv->is_expired_date = true;
        gtk_widget_set_visible(GTK_WIDGET(priv->imgbtn_batch_download), false);
    }

    if (lbl_reduce) {
        gtk_widget_destroy(lbl_reduce);
        lbl_reduce = NULL;
    }

    return row;
}

Ftc::Core::GlobalStruct::FtcRequest* 
ftc_home_recv_list_row_get_ftc_request(FtcHomeRecvListRow *row)
{
    FtcHomeRecvListRowPrivate *priv = NULL;
    Ftc::Core::GlobalStruct::FtcRequest *rv = NULL;
    if (! FTC_IS_HOME_RECV_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = priv->ftc_request;
    return rv;
}

RequestDownloadState*
ftc_home_recv_list_row_get_request_download_state(FtcHomeRecvListRow *row)
{
    FtcHomeRecvListRowPrivate *priv = NULL;
    RequestDownloadState *rv = NULL;
    if (! FTC_IS_HOME_RECV_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = priv->download_state;
    return rv;
}

static void 
ftc_home_recv_list_row_on_button_press_event_box_batch_download(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeRecvListRow *row = NULL;
    FtcHomeRecvListRowPrivate *priv = NULL;
    GdkEventButton *event_button = NULL;

    if (! FTC_IS_HOME_RECV_LIST_ROW(data)) {
        return;
    }
    row = FTC_HOME_RECV_LIST_ROW(data);

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        ftc_home_recv_list_sub_page_list_box_select_row_and_batch_download(priv->recvlist_sub_page, priv->ftc_request);
    }
}

void                    
ftc_home_recv_list_row_update_title_read(FtcHomeRecvListRow *row)
{
    FtcHomeRecvListRowPrivate *priv = NULL;

    if (! FTC_IS_HOME_RECV_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_title), "label_recv_info_title"); 
}

void
ftc_home_recv_list_row_set_visible_progress(FtcHomeRecvListRow *row, bool is_visible)
{
    FtcHomeRecvListRowPrivate *priv = NULL;

    if (! FTC_IS_HOME_RECV_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    gtk_widget_set_visible(GTK_WIDGET(priv->box_progress_area), is_visible);

    if (priv->is_expired_date == false) {
        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_batch_download), !is_visible);
    }
    
}

void                    
ftc_home_recv_list_row_set_count_progress(FtcHomeRecvListRow *row, int count, int total_count)
{
    FtcHomeRecvListRowPrivate *priv = NULL;
    std::string str;
    double value = 0;

    if (! FTC_IS_HOME_RECV_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    if (total_count >= 0) {
        priv->count = 0;
        priv->total_count = total_count;
    }
    if (count >= 0) {
        priv->count = count;
    }

    str = std::to_string(priv->count) + "/" + std::to_string(priv->total_count);
    gtk_label_set_text(priv->lbl_progress, str.c_str());

    value = ((double)priv->count / priv->total_count) * 100;
    gtk_progress_bar_set_fraction(priv->pbar_progress, value);
    
}

void
ftc_home_recv_list_row_add_count_progress(FtcHomeRecvListRow *row)
{
    FtcHomeRecvListRowPrivate *priv = NULL;
    std::string str;
    double value = 0;

    if (! FTC_IS_HOME_RECV_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    priv->count++;

    str = std::to_string(priv->count) + "/" + std::to_string(priv->total_count);
    gtk_label_set_text(priv->lbl_progress, str.c_str());

    value = ((double)priv->count / priv->total_count);
    if (value > 0) {
        gtk_progress_bar_set_fraction(priv->pbar_progress, value);
    }
}

void
ftc_home_recv_list_row_set_progress_background_value(FtcHomeRecvListRow *row, const char *request_info_file_list_uid, int progress)
{
    FtcHomeRecvListRowPrivate *priv = NULL;

    if (! FTC_IS_HOME_RECV_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    if (! request_info_file_list_uid) {
        return;
    }

    if (progress > 100) {
        progress = 100;
    } else if (progress < 0) {
        progress = 0;
    }

    for (auto &it : priv->download_state->vc_file_down_status) {
        if (it.request_info_file_list_uid == request_info_file_list_uid) {
            if (it.is_downloading == true) {
                it.progress = progress;
            }
            break;
        }
    }
}

void
ftc_home_recv_list_row_download_complete(FtcHomeRecvListRow *row)
{
    FtcHomeRecvListRowPrivate *priv = NULL;

    if (! FTC_IS_HOME_RECV_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeRecvListRowPrivate*)ftc_home_recv_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    priv->download_state->is_downloading = false;
    for (auto &it : priv->download_state->vc_file_down_status) {
        it.is_downloading = false;
        it.progress = 0;
    }
}