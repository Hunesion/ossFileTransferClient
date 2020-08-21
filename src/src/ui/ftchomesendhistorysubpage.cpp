#include "ftchomesendhistorysubpage.h"
#include "ftchomesendhistoryrow.h"
#include "ftcmainwindow.h"
#include "../core/TimeUtils.h"
#include <mutex>
#include <ctime>
#include <time.h>


struct _FtcHomeSendHistorySubPage
{
    GtkBox parent;
};

typedef struct _FtcHomeSendHistorySubPagePrivate FtcHomeSendHistorySubPagePrivate;

struct _FtcHomeSendHistorySubPagePrivate
{
    FtcHomePage *home_page;

    GtkScrolledWindow *scrolled_window;

    GtkListBox  *listbox_send_history;

    int page_count;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomeSendHistorySubPage, ftc_home_send_history_sub_page, GTK_TYPE_BOX)

typedef struct _ListBoxFindRowParam
{
    const Ftc::Core::GlobalStruct::FtcRequest *ftc_request;
    const char *request_info_uid;
    GtkListBoxRow *find_row;
    bool is_success;

    _ListBoxFindRowParam()
    {
        ftc_request = NULL;
        find_row = NULL;
        is_success = false;
    }
} ListBoxFindRowParam;

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_send_history_sub_page_scrolled_window_edge_reached(GtkScrolledWindow *scrolled_window, GtkPositionType pos, gpointer data);

static void ftc_home_send_history_sub_page_on_row_selected_list_box(GtkListBox *box, GtkListBoxRow *row, gpointer data);

static void ftc_home_send_history_sub_page_list_box_update_header(GtkListBoxRow *row, GtkListBoxRow *before, gpointer data);

static void ftc_home_send_history_sub_page_list_box_row_destroy_notify(gpointer data);

static bool ftc_home_send_history_sub_page_request_history_list(FtcHomeSendHistorySubPage *page, int page_count);

static int ftc_home_send_history_sub_page_get_send_request_new_count(FtcHomeSendHistorySubPage *page);

static void ftc_home_send_history_sub_page_list_box_add(FtcHomeSendHistorySubPage *page, JsonObject *obj);

static void ftc_home_send_history_sub_page_list_box_clear(FtcHomeSendHistorySubPage *page);

static void ftc_home_send_history_sub_page_list_box_clear_callback(GtkWidget *widget, gpointer data);

static int  ftc_home_send_history_sub_page_list_box_get_size(FtcHomeSendHistorySubPage *page);

static void ftc_home_send_history_sub_page_list_box_get_size_callback(GtkWidget *widget, gpointer data);

static GtkListBoxRow* ftc_home_send_history_sub_page_list_box_find_row_for_ftc_request(FtcHomeSendHistorySubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request);

static void ftc_home_send_history_sub_page_list_box_find_row_for_ftc_request_callback(GtkWidget *widget, gpointer data);

static GtkListBoxRow* ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid(FtcHomeSendHistorySubPage *page, const char *request_info_uid);

static void ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid_callback(GtkWidget *widget, gpointer data);

static void ftc_home_send_history_sub_page_select_list_row(FtcHomeSendHistorySubPage *page, FtcHomeSendHistoryRow *row, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, RequestDownloadState *request_down_state);

///////////////////////////////////////////////////////////////////

static void
ftc_home_send_history_sub_page_init (FtcHomeSendHistorySubPage *page)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private (page);

    //  Signal 연결
    g_signal_connect(priv->listbox_send_history, "row-selected", G_CALLBACK(ftc_home_send_history_sub_page_on_row_selected_list_box), page);
    g_signal_connect(priv->scrolled_window, "edge-reached", G_CALLBACK(ftc_home_send_history_sub_page_scrolled_window_edge_reached), page);
    gtk_list_box_set_header_func(priv->listbox_send_history, ftc_home_send_history_sub_page_list_box_update_header, page, ftc_home_send_history_sub_page_list_box_row_destroy_notify);

    //  ListBox 초기화
    gtk_list_box_set_selection_mode(priv->listbox_send_history, GTK_SELECTION_SINGLE);

    //  CSS 적용
    //
    gtk_widget_set_name(GTK_WIDGET(page), "default_background_page");
    gtk_widget_set_name(GTK_WIDGET(priv->listbox_send_history), "box_background_white");
}

static void
ftc_home_send_history_sub_page_dispose (GObject *object)
{
    FtcHomeSendHistorySubPagePrivate *priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(FTC_HOME_SEND_HISTORY_SUB_PAGE(object));

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);

    G_OBJECT_CLASS(ftc_home_send_history_sub_page_parent_class)->dispose(object);
}

static void
ftc_home_send_history_sub_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_home_send_history_sub_page_parent_class)->finalize(object);
}

static void
ftc_home_send_history_sub_page_class_init (FtcHomeSendHistorySubPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_send_history_sub_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_send_history_sub_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-send-history-sub-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendHistorySubPage, scrolled_window);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendHistorySubPage, listbox_send_history);
}

FtcHomeSendHistorySubPage *
ftc_home_send_history_sub_page_new (FtcHomePage *home_page)
{
    FtcHomeSendHistorySubPage *page = FTC_HOME_SEND_HISTORY_SUB_PAGE(g_object_new (FTC_HOME_SEND_HISTORY_SUB_PAGE_TYPE, NULL));
    FtcHomeSendHistorySubPagePrivate *priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private (page);

    priv->home_page = home_page;
    ftc_home_send_history_sub_page_get_send_history_list(page);
    
    return page;
}

static void 
ftc_home_send_history_sub_page_scrolled_window_edge_reached(GtkScrolledWindow *scrolled_window, GtkPositionType pos, gpointer data)
{
    FtcHomeSendHistorySubPage *page = NULL;
    FtcHomeSendHistorySubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_SEND_HISTORY_SUB_PAGE(data);

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (pos == GtkPositionType::GTK_POS_BOTTOM) {
        if (ftc_home_send_history_sub_page_request_history_list(page, priv->page_count) == true) {
            priv->page_count++;
        }
    }
}

static void 
ftc_home_send_history_sub_page_on_row_selected_list_box(GtkListBox *box, GtkListBoxRow *row, gpointer data)
{
    FtcHomeSendHistorySubPage *page = NULL;
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    UpdatePageEventParam update_param;
    FtcHomeSendHistoryRow *list_row = NULL;
    Ftc::Core::GlobalStruct::FtcRequest *ftc_request = NULL;
    RequestDownloadState *request_down_state = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_SEND_HISTORY_SUB_PAGE(data);

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! row) {
        return;
    }

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (! event_mgr) {
        return;
    }

    list_row = FTC_HOME_SEND_HISTORY_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_HOME_SEND_HISTORY_ROW(list_row)) {
        return ;
    }

    ftc_request = ftc_home_send_history_row_get_ftc_request(list_row);
    if (! ftc_request) {
        return;
    }
    request_down_state = ftc_home_send_history_row_get_request_download_state(list_row);
    if (! request_down_state) {
        return;
    }

    ftc_home_send_history_sub_page_select_list_row(page, list_row, ftc_request, request_down_state);

    update_param.ftc_object = ftc_ui_get_main_window(); /* MAIN WINDOW */
    update_param.page_name = FTC_VIEW_WINDOW_PAGE_SEND_HISTORY_DETAIL;

    event_mgr->dispatchEvent(FTC_UI_UPDATE_MAIN_STACK_EVENT, &update_param);
}

static void 
ftc_home_send_history_sub_page_list_box_update_header(GtkListBoxRow *row, GtkListBoxRow *before, gpointer data)
{
}

static void 
ftc_home_send_history_sub_page_list_box_row_destroy_notify(gpointer data)
{
}

static bool
ftc_home_send_history_sub_page_request_history_list(FtcHomeSendHistorySubPage *page, int page_count)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    FtcHomeSendHistoryRow *list_row = NULL;
    JsonNode *node = NULL;
    JsonArray *list = NULL;
    int list_count = 0;
    bool rv = false;

    //  기본적으로 한달 치의 리스트를 가져온다.
    std::tm to_date_tm = Ftc::Core::TimeUtils::now();
    std::tm from_date_tm = Ftc::Core::TimeUtils::addDay(to_date_tm, -30);

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }
    
    auto resp = Ftc::Core::Request::requestSelectSendFileList_ajax(page_count, 
                    Ftc::Core::TimeUtils::getDateFormat(from_date_tm, "%Y%02m%02d").c_str(),
                    Ftc::Core::TimeUtils::getDateFormat(to_date_tm, "%Y%02m%02d").c_str());

    for (int i = 0 ; i < list_count; i++) {
        ftc_home_send_history_sub_page_list_box_add(page, json_array_get_object_element(list, i));
    }
    rv = true;
    return rv;
}

void 
ftc_home_send_history_sub_page_get_send_history_list(FtcHomeSendHistorySubPage *page)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    FtcHomeSendHistoryRow *list_row = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    SendHistoryCountEventParam param;
    int new_count = 0;
    
    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    // GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(priv->scrolled_window);
    // gtk_adjustment_set_value(adj, 0);

    priv->page_count = 1;
    ftc_home_send_history_sub_page_list_box_clear(page);

    if (ftc_home_send_history_sub_page_request_history_list(page, priv->page_count) == true) {
        priv->page_count++;

        new_count = ftc_home_send_history_sub_page_get_send_request_new_count(page);

        event_mgr = Ftc::Core::EventManager::getInstance();
        if (event_mgr) {
            param.count = new_count;
            param.ftc_object = priv->home_page;
            event_mgr->dispatchEvent(FTC_UI_SEND_HISTORY_NEW_COUNT, &param);
        }
    }
}


static int  
ftc_home_send_history_sub_page_get_send_request_new_count(FtcHomeSendHistorySubPage *page)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    JsonNode *node = NULL;
    bool result = false;
    int rv = 0;
    //  기본적으로 한달 치의 리스트를 가져온다.
    std::tm to_date_tm = Ftc::Core::TimeUtils::now();
    std::tm from_date_tm = Ftc::Core::TimeUtils::addDay(to_date_tm, -30);
    
    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    result = Ftc::Core::Request::requestLeftmenuCount_ajax("send", Ftc::Core::TimeUtils::getDateFormat(from_date_tm, "%Y%02m%02d").c_str(), Ftc::Core::TimeUtils::getDateFormat(to_date_tm, "%Y%02m%02d").c_str());
    if (result) {
        const Ftc::Core::LeftMenuCount &left_menu_count = Ftc::Core::GlobalVar::getLeftMenuCount();
        rv = left_menu_count.getSendCnt();
    }

    return rv;
}

static void 
ftc_home_send_history_sub_page_list_box_add(FtcHomeSendHistorySubPage *page, JsonObject *obj)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    FtcHomeSendHistoryRow *list_row = NULL;
    const char *tmp = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }

    if (!obj) {
        FTC_LOG_DEBUG("ftc_home_send_history_sub_page_list_box_add JsonObject(list) null!");
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);

    Ftc::Core::GlobalStruct::FtcRequest *ftc_request = new Ftc::Core::GlobalStruct::FtcRequest();
    list_row = ftc_home_send_history_row_new(page, ftc_request);
    gtk_list_box_insert(priv->listbox_send_history, GTK_WIDGET(list_row), -1);
}

static void 
ftc_home_send_history_sub_page_list_box_clear(FtcHomeSendHistorySubPage *page)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send_history), ftc_home_send_history_sub_page_list_box_clear_callback, page);
}

static void 
ftc_home_send_history_sub_page_list_box_clear_callback(GtkWidget *widget, gpointer data)
{
    FtcHomeSendHistorySubPage *page = NULL;
    FtcHomeSendHistorySubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_SEND_HISTORY_SUB_PAGE(data);

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! widget) {
        return;
    }

    gtk_container_remove(GTK_CONTAINER(priv->listbox_send_history), widget);
}

static int  
ftc_home_send_history_sub_page_list_box_get_size(FtcHomeSendHistorySubPage *page)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    int rv = 0;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return rv;
    }
    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send_history), ftc_home_send_history_sub_page_list_box_get_size_callback, &rv);

    return rv;
}

static void 
ftc_home_send_history_sub_page_list_box_get_size_callback(GtkWidget *widget, gpointer data)
{
    int *list_total_count_ptr = NULL;

    if (! data || ! widget) {
        return;
    }

    list_total_count_ptr = (int*)data;
    (*list_total_count_ptr)++;
}

void                        
ftc_home_send_history_sub_page_list_box_select_row_and_batch_download(FtcHomeSendHistorySubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    FtcHomeSendHistoryRow *list_row = NULL;
    GtkListBoxRow *row = NULL;
    RequestDownloadState *request_down_state = NULL;
    

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }
    if (! ftc_request) {
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
        
    }

    row = ftc_home_send_history_sub_page_list_box_find_row_for_ftc_request(page, ftc_request);
    if (! row) {
        return;
    }
    list_row = FTC_HOME_SEND_HISTORY_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_HOME_SEND_HISTORY_ROW(list_row)) {
        return;
    }
    request_down_state = ftc_home_send_history_row_get_request_download_state(list_row);
    
    if (! request_down_state) {
        return;
    }

    //gtk_list_box_select_row(priv->listbox_send_history, list_box_row);
    ftc_home_send_history_sub_page_select_list_row(page, list_row, ftc_request, request_down_state);

}


static GtkListBoxRow* 
ftc_home_send_history_sub_page_list_box_find_row_for_ftc_request(FtcHomeSendHistorySubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    GtkListBoxRow *rv = NULL;
    ListBoxFindRowParam param;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return rv;
    }
    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    if (! ftc_request) {
        return rv;
    }

    param.ftc_request = ftc_request;

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send_history), ftc_home_send_history_sub_page_list_box_find_row_for_ftc_request_callback, &param);

    if (param.is_success) {
        rv = param.find_row;
    }

    return rv;
}

static void 
ftc_home_send_history_sub_page_list_box_find_row_for_ftc_request_callback(GtkWidget *widget, gpointer data)
{
    ListBoxFindRowParam *param = NULL;
    GtkListBoxRow* row = NULL;
    FtcHomeSendHistoryRow *recv_list_row = NULL;
    Ftc::Core::GlobalStruct::FtcRequest *ftc_request_cmp = NULL;

    if (! widget || ! data) {
        return;
    }

    param = (ListBoxFindRowParam*)data;
    row = GTK_LIST_BOX_ROW(widget);

    recv_list_row = FTC_HOME_SEND_HISTORY_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_HOME_SEND_HISTORY_ROW(recv_list_row)) {
        return;
    }

    ftc_request_cmp = ftc_home_send_history_row_get_ftc_request(recv_list_row);
    if (! ftc_request_cmp) {
        return;
    }

    if (ftc_request_cmp->request_info_uid == param->ftc_request->request_info_uid) {
        param->find_row = row;
        param->is_success = true;
    }
}

static GtkListBoxRow*
ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid(FtcHomeSendHistorySubPage *page, const char *request_info_uid)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    GtkListBoxRow *rv = NULL;
    ListBoxFindRowParam param;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return rv;
    }
    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    if (! request_info_uid) {
        return rv;
    }

    param.request_info_uid = request_info_uid;

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send_history), ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid_callback, &param);

    if (param.is_success) {
        rv = param.find_row;
    }

    return rv;
}

static void 
ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid_callback(GtkWidget *widget, gpointer data)
{
    ListBoxFindRowParam *param = NULL;
    GtkListBoxRow* row = NULL;
    FtcHomeSendHistoryRow *recv_list_row = NULL;
    Ftc::Core::GlobalStruct::FtcRequest *ftc_request_cmp = NULL;

    if (! widget || ! data) {
        return;
    }

    param = (ListBoxFindRowParam*)data;
    row = GTK_LIST_BOX_ROW(widget);

    recv_list_row = FTC_HOME_SEND_HISTORY_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_HOME_SEND_HISTORY_ROW(recv_list_row)) {
        return;
    }

    ftc_request_cmp = ftc_home_send_history_row_get_ftc_request(recv_list_row);
    if (! ftc_request_cmp) {
        return;
    }

    if (ftc_request_cmp->request_info_uid == param->request_info_uid) {
        param->find_row = row;
        param->is_success = true;
    } 
}

void 
ftc_home_send_history_sub_page_list_box_unselect_all(FtcHomeSendHistorySubPage *page)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    gtk_list_box_unselect_all(priv->listbox_send_history);
}

static void 
ftc_home_send_history_sub_page_select_list_row(FtcHomeSendHistorySubPage *page, FtcHomeSendHistoryRow *row, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, RequestDownloadState *request_down_state)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    SendHistoryCountEventParam param;
    Ftc::Core::EventManager *event_mgr = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! row) {
        return;
    }

    if (! ftc_request || ! request_down_state) {
        return;
    }
    
    if (ftc_request->is_read == false) {
        event_mgr = Ftc::Core::EventManager::getInstance();
        if (! event_mgr) {
            return;
        }
    
        param.ftc_object = priv->home_page;
        param.count = ftc_home_send_history_sub_page_get_send_request_new_count(page);
        event_mgr->dispatchEvent(FTC_UI_SEND_HISTORY_NEW_COUNT, &param);

        ftc_home_send_history_row_update_title_read(row);
    }
}

void                        
ftc_home_send_history_sub_page_set_visible_progress(FtcHomeSendHistorySubPage *page, const char *request_info_uid, bool is_visible, int total_count)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    FtcHomeSendHistoryRow *list_row = NULL;
    GtkListBoxRow *row = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    row = ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid(page, request_info_uid);
    if (! row) {
        return;
    }
    
    list_row = FTC_HOME_SEND_HISTORY_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_HOME_SEND_HISTORY_ROW(list_row)) {
        return;
    }

    ftc_home_send_history_row_set_visible_progress(list_row, is_visible);
    ftc_home_send_history_row_set_count_progress(list_row, -1, total_count);
}

void
ftc_home_send_history_sub_page_set_progress_background_value(FtcHomeSendHistorySubPage *page, const char *request_info_uid, const char *request_info_file_list_uid, int progress)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    FtcHomeSendHistoryRow *list_row = NULL;
    GtkListBoxRow *row = NULL;
    RequestDownloadState *request_down_state = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! request_info_uid || ! request_info_file_list_uid) {
        return;
    }

    row = ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid(page, request_info_uid);
    if (! row) {
        return;
    }
    list_row = FTC_HOME_SEND_HISTORY_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_HOME_SEND_HISTORY_ROW(list_row)) {
        return;
    }

    ftc_home_send_history_row_set_progress_background_value(list_row, request_info_file_list_uid, progress);
}

void
ftc_home_send_history_sub_page_update_total_progress(FtcHomeSendHistorySubPage *page, const char *request_info_uid)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    FtcHomeSendHistoryRow *list_row = NULL;
    GtkListBoxRow *row = NULL;
    RequestDownloadState *request_down_state = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! request_info_uid) {
        return;
    }

    row = ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid(page, request_info_uid);
    if (! row) {
        return;
    }
    list_row = FTC_HOME_SEND_HISTORY_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_HOME_SEND_HISTORY_ROW(list_row)) {
        return;
    }

    ftc_home_send_history_row_add_count_progress(list_row);
}

void
ftc_home_send_history_sub_page_download_complete(FtcHomeSendHistorySubPage *page, const char *request_info_uid)
{
    FtcHomeSendHistorySubPagePrivate *priv = NULL;
    FtcHomeSendHistoryRow *list_row = NULL;
    GtkListBoxRow *row = NULL;
    RequestDownloadState *request_down_state = NULL;

    if (! FTC_IS_HOME_SEND_HISTORY_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendHistorySubPagePrivate*)ftc_home_send_history_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! request_info_uid) {
        return;
    }

    row = ftc_home_send_history_sub_page_list_box_find_row_for_request_info_uid(page, request_info_uid);
    if (! row) {
        return;
    }
    list_row = FTC_HOME_SEND_HISTORY_ROW(gtk_bin_get_child(GTK_BIN(row)));
    if (! FTC_IS_HOME_SEND_HISTORY_ROW(list_row)) {
        return;
    }

    ftc_home_send_history_row_download_complete(list_row);
}