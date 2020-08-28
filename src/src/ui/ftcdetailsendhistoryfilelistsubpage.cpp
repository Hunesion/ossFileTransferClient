#include "ftcdetailsendhistoryfilelistsubpage.h"
#include "ftcdetailsendhistoryfilelistrow.h"


struct _FtcDetailSendHistoryFileListSubPage
{
    GtkBox parent;
};

typedef struct _FtcDetailSendHistoryFileListSubPagePrivate FtcDetailSendHistoryFileListSubPagePrivate;

struct _FtcDetailSendHistoryFileListSubPagePrivate
{
    Ftc::Core::GlobalStruct::FtcRequest         *ftc_request;
    RequestDownloadState *request_down_state;

    GtkBox              *box_list_area;
    GtkBox              *box_list_header;
    GtkBox              *box_scrolled_area;
    GtkCheckButton      *ckbtn_all_check;
    GtkLabel            *lbl_file_name;
    GtkLabel            *lbl_file_size;
    GtkLabel            *lbl_download_count;
    GtkScrolledWindow   *scrolled_window;
    GtkListBox          *list_box_file;
    
    
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcDetailSendHistoryFileListSubPage, ftc_detail_send_history_file_list_sub_page, GTK_TYPE_BOX)

typedef struct _ListBoxFindRowParam
{
    const char *request_info_file_list_uid;
    GtkListBoxRow *find_row;
    bool is_success;

    _ListBoxFindRowParam()
    {
        request_info_file_list_uid = NULL;
        find_row = NULL;
        is_success = false;
    }
} ListBoxFindRowParam;

////////////////////////    Private 선언   ////////////////////////
static void ftc_detail_send_history_file_list_sub_page_on_toggled_ckbtn_all_check(GtkToggleButton *togglebutton, gpointer data);

static void ftc_detail_send_history_file_list_sub_page_list_box_clear(FtcDetailSendHistoryFileListSubPage *page);

static void ftc_detail_send_history_file_list_sub_page_list_box_clear_callback(GtkWidget *widget, gpointer data);

static void ftc_detail_send_history_file_list_sub_page_get_checked_file_uid_callback(GtkWidget *widget, gpointer data);

static void ftc_detail_send_history_file_list_sub_page_get_all_file_uid_callback(GtkWidget *widget, gpointer data);

static const char* ftc_detail_send_history_file_list_sub_page_get_save_directory(FtcDetailSendHistoryFileListSubPage *page);

static void ftc_detail_send_history_file_list_sub_page_set_all_check_active_callback(GtkWidget *widget, gpointer data);

static void ftc_detail_send_history_file_list_sub_page_set_visible_progress(FtcDetailSendHistoryFileListSubPage *page, const char *request_info_file_list_uid, bool is_visible);

static GtkListBoxRow* ftc_detail_send_history_file_list_sub_page_list_box_find_row(FtcDetailSendHistoryFileListSubPage *page, const char *request_info_file_list_uid);

static void ftc_detail_send_history_file_list_sub_page_list_box_find_row_callback(GtkWidget *widget, gpointer data);
///////////////////////////////////////////////////////////////////

static void
ftc_detail_send_history_file_list_sub_page_init (FtcDetailSendHistoryFileListSubPage *page)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;

    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private (page);

    //  Signal (Event) 함수 연결
    //  ckbtn_all_check toggle active
    g_signal_connect(priv->ckbtn_all_check, "toggled", G_CALLBACK(ftc_detail_send_history_file_list_sub_page_on_toggled_ckbtn_all_check), page);

    //  Widget 크기 설정
    //
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_list_area), 400, 520);
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_list_header), 400, 30);
    gtk_widget_set_size_request(GTK_WIDGET(priv->scrolled_window), 400, 490);


    //  CSS 설정
    //  Background 색상 설정
    gtk_widget_set_name(GTK_WIDGET(priv->box_list_header), "box_detail_header_sub_title");
    gtk_widget_set_name(GTK_WIDGET(priv->list_box_file), "default_background_page");
    
    

    //  위젯설정
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_file_name), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_file_size), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_download_count), "label_recv_info_title");

    gtk_list_box_set_selection_mode(priv->list_box_file, GTK_SELECTION_NONE);
}

static void
ftc_detail_send_history_file_list_sub_page_dispose (GObject *object)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv;
    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private(FTC_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(object));

    priv->ftc_request = NULL;
    priv->request_down_state = NULL;

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);
    
    G_OBJECT_CLASS(ftc_detail_send_history_file_list_sub_page_parent_class)->dispose(object);
}

static void
ftc_detail_send_history_file_list_sub_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_detail_send_history_file_list_sub_page_parent_class)->finalize(object);
}

static void
ftc_detail_send_history_file_list_sub_page_class_init (FtcDetailSendHistoryFileListSubPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_detail_send_history_file_list_sub_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_detail_send_history_file_list_sub_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-detail-send-history-file-list-sub-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, box_list_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, box_list_header);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, box_scrolled_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, ckbtn_all_check);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, lbl_file_name);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, lbl_file_size);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, lbl_download_count);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, scrolled_window);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendHistoryFileListSubPage, list_box_file);
    
}

FtcDetailSendHistoryFileListSubPage *
ftc_detail_send_history_file_list_sub_page_new ()
{
    FtcDetailSendHistoryFileListSubPage *page = FTC_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(g_object_new (FTC_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE_TYPE, NULL));
    FtcDetailSendHistoryFileListSubPagePrivate *priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private (page);

    return page;
}


void                    
ftc_detail_send_history_file_list_sub_page_set_data(FtcDetailSendHistoryFileListSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, RequestDownloadState *request_down_state)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;
    std::string str = "";
    bool is_expired_date = false;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return;
    }

    if (! ftc_request || ! request_down_state) {
        return;
    }

    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }
    priv->ftc_request = ftc_request;
    priv->request_down_state = request_down_state;

    //  전체선택 해제
    //
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->ckbtn_all_check), false);

    ftc_detail_send_history_file_list_sub_page_list_box_clear(page);

    //  유효기간 체크
    is_expired_date = ftc_ui_is_expired_date_request_info(ftc_request->download_expire_dt);

    for (auto it = priv->ftc_request->vc_file_info.begin(); it != priv->ftc_request->vc_file_info.end(); it++) {
        FtcDetailSendHistoryFileListRow *file_list_row = ftc_detail_send_history_file_list_row_new();
        ftc_detail_send_history_file_list_row_set_data(file_list_row, *it, is_expired_date);
        gtk_list_box_insert(priv->list_box_file, GTK_WIDGET(file_list_row), -1);
    }
}

static void 
ftc_detail_send_history_file_list_sub_page_on_toggled_ckbtn_all_check(GtkToggleButton *togglebutton, gpointer data)
{
    FtcDetailSendHistoryFileListSubPage *page = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(data);

    if (! togglebutton) {
        return;
    }

    if (gtk_toggle_button_get_active(togglebutton) == true) {
        ftc_detail_send_history_file_list_sub_page_set_all_check_active(page, true);
    } else {
        ftc_detail_send_history_file_list_sub_page_set_all_check_active(page, false);
    }
}

static void 
ftc_detail_send_history_file_list_sub_page_list_box_clear(FtcDetailSendHistoryFileListSubPage *page)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->list_box_file), ftc_detail_send_history_file_list_sub_page_list_box_clear_callback, page);
}

static void 
ftc_detail_send_history_file_list_sub_page_list_box_clear_callback(GtkWidget *widget, gpointer data)
{
    FtcDetailSendHistoryFileListSubPage *page = NULL;
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(data);

    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    if (! widget) {
        return;
    }

    gtk_container_remove(GTK_CONTAINER(priv->list_box_file), widget);
}

GSList* 
ftc_detail_send_history_file_list_sub_page_get_checked_file_uid(FtcDetailSendHistoryFileListSubPage *page)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;
    GSList *rv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private (page);
    if (! priv) {
        return rv;
    }

    rv = g_slist_alloc();
    if (! rv) {
        return rv;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->list_box_file), ftc_detail_send_history_file_list_sub_page_get_checked_file_uid_callback, rv);

    return rv;
}

static void 
ftc_detail_send_history_file_list_sub_page_get_checked_file_uid_callback(GtkWidget *widget, gpointer data)
{
    FtcDetailSendHistoryFileListRow *list_row = NULL;
    GSList *file_uid_list = NULL;

    if (! widget || ! data) {
        return;
    }

    list_row = FTC_DETAIL_SEND_HISTORY_FILE_LIST_ROW(gtk_bin_get_child(GTK_BIN(widget)));
    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_ROW(list_row)) {
        return;
    }

    file_uid_list = (GSList*)data;

    if (ftc_detail_send_history_file_list_row_get_check_active(list_row) == true) {
        file_uid_list = g_slist_append(file_uid_list, (gpointer)ftc_detail_send_history_file_list_row_get_file_uid(list_row));
    }
}

GSList*                            
ftc_detail_send_history_file_list_sub_page_get_all_file_uid(FtcDetailSendHistoryFileListSubPage *page)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;
    GSList *rv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private (page);
    if (! priv) {
        return rv;
    }

    rv = g_slist_alloc();
    if (! rv) {
        return rv;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->list_box_file), ftc_detail_send_history_file_list_sub_page_get_all_file_uid_callback, rv);

    return rv;
}

static void
ftc_detail_send_history_file_list_sub_page_get_all_file_uid_callback(GtkWidget *widget, gpointer data)
{
    FtcDetailSendHistoryFileListRow *list_row = NULL;
    GSList *file_uid_list = NULL;

    if (! widget || ! data) {
        return;
    }

    list_row = FTC_DETAIL_SEND_HISTORY_FILE_LIST_ROW(gtk_bin_get_child(GTK_BIN(widget)));
    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_ROW(list_row)) {
        return;
    }

    file_uid_list = (GSList*)data;

    file_uid_list = g_slist_append(file_uid_list, (gpointer)ftc_detail_send_history_file_list_row_get_file_uid(list_row));
}

void 
ftc_detail_send_history_file_list_sub_page_set_all_check_active(FtcDetailSendHistoryFileListSubPage *page, bool check)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->list_box_file), ftc_detail_send_history_file_list_sub_page_set_all_check_active_callback, &check);
}

static void 
ftc_detail_send_history_file_list_sub_page_set_all_check_active_callback(GtkWidget *widget, gpointer data)
{
    FtcDetailSendHistoryFileListRow *list_row;
    bool *check = NULL;

    if (! widget || ! data) {
        return;
    }

    check = (bool*)data;

    list_row = FTC_DETAIL_SEND_HISTORY_FILE_LIST_ROW(gtk_bin_get_child(GTK_BIN(widget)));
    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_ROW(list_row)) {
        return;
    }
    
    ftc_detail_send_history_file_list_row_set_check_active(list_row, *check);
}

void
ftc_detail_send_history_file_list_sub_page_update_activation_view(FtcDetailSendHistoryFileListSubPage *page)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! priv->request_down_state) {
        return;
    }

    if (priv->request_down_state->is_downloading == true) {
        for (auto &it : priv->request_down_state->vc_file_down_status) {
            if (it.is_downloading == true) {
                ftc_detail_send_history_file_list_sub_page_set_visible_progress(page, it.request_info_file_list_uid.c_str(), true);
                ftc_detail_send_history_file_list_sub_page_set_progress_value(page, it.request_info_file_list_uid.c_str(), it.progress);
            }
        }
    } else {
        for (auto &it : priv->request_down_state->vc_file_down_status) {
            ftc_detail_send_history_file_list_sub_page_set_visible_progress(page, it.request_info_file_list_uid.c_str(), false);
        }
    }
}

static void 
ftc_detail_send_history_file_list_sub_page_set_visible_progress(FtcDetailSendHistoryFileListSubPage *page, const char *request_info_file_list_uid, bool is_visible)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;
    FtcDetailSendHistoryFileListRow *list_row = NULL;
    GtkListBoxRow *row = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    row = ftc_detail_send_history_file_list_sub_page_list_box_find_row(page, request_info_file_list_uid);
    if (! row) {
        return;
    }
    list_row = FTC_DETAIL_SEND_HISTORY_FILE_LIST_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_ROW(list_row)) {
        return;
    }

    ftc_detail_send_history_file_list_row_set_visible_progress(list_row, is_visible);
}

static GtkListBoxRow*
ftc_detail_send_history_file_list_sub_page_list_box_find_row(FtcDetailSendHistoryFileListSubPage *page, const char *request_info_file_list_uid)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;
    GtkListBoxRow *rv = NULL;
    ListBoxFindRowParam param;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    if (! request_info_file_list_uid) {
        return rv;
    }

    param.request_info_file_list_uid = request_info_file_list_uid;

    gtk_container_foreach(GTK_CONTAINER(priv->list_box_file), ftc_detail_send_history_file_list_sub_page_list_box_find_row_callback, &param);

    rv = param.find_row;
    return rv;
}

static void 
ftc_detail_send_history_file_list_sub_page_list_box_find_row_callback(GtkWidget *widget, gpointer data)
{
    ListBoxFindRowParam *param = NULL;
    GtkListBoxRow* row = NULL;
    FtcDetailSendHistoryFileListRow *file_list_row = NULL;
    const char *request_info_file_list_uid_cmp = NULL;

    if (! widget || ! data) {
        return;
    }

    param = (ListBoxFindRowParam*)data;
    row = GTK_LIST_BOX_ROW(widget);

    file_list_row = FTC_DETAIL_SEND_HISTORY_FILE_LIST_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_ROW(file_list_row)) {
        return;
    }

    request_info_file_list_uid_cmp = ftc_detail_send_history_file_list_row_get_file_uid(file_list_row);
    if (! request_info_file_list_uid_cmp) {
        return;
    }

    if (strcmp(request_info_file_list_uid_cmp, param->request_info_file_list_uid) == 0) {
        param->find_row = row;
        param->is_success = true;
    }
}

void
ftc_detail_send_history_file_list_sub_page_set_progress_value(FtcDetailSendHistoryFileListSubPage *page, const char *request_info_file_list_uid, int progress)
{
    FtcDetailSendHistoryFileListSubPagePrivate *priv = NULL;
    FtcDetailSendHistoryFileListRow *list_row = NULL;
    GtkListBoxRow *row = NULL;

    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcDetailSendHistoryFileListSubPagePrivate*)ftc_detail_send_history_file_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    for (auto &it : priv->request_down_state->vc_file_down_status) {
        if (it.request_info_file_list_uid == request_info_file_list_uid) {
            if (it.is_downloading == false) {
                return;
            }
            
            it.progress = progress;
            break;
        }
    }

    row = ftc_detail_send_history_file_list_sub_page_list_box_find_row(page, request_info_file_list_uid);
    if (! row) {
        return;
    }
    list_row = FTC_DETAIL_SEND_HISTORY_FILE_LIST_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_DETAIL_SEND_HISTORY_FILE_LIST_ROW(list_row)) {
        return;
    }

    ftc_detail_send_history_file_list_row_set_progress(list_row, progress);
}