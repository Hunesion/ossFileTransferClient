#include "ftcdetailrecvpage.h"
#include "ftcdetailrecvfilelistsubpage.h"
#include "ftcdetailrecvrequestinfosubpage.h"
#include <sstream>
#include <set>


struct _FtcDetailRecvPage
{
    GtkBox parent;
};

typedef struct _FtcDetailRecvPagePrivate FtcDetailRecvPagePrivate;

struct _FtcDetailRecvPagePrivate
{
    FtcHomeRecvListSubPage *recvlist_sub_page;
    FtcDetailRecvFileListSubPage *filelist_sub_page;
    FtcDetailRecvRequestInfoSubPage *requestinfo_sub_page;

    Ftc::Core::GlobalStruct::FtcRequest *ftc_request;
    RequestDownloadState *request_down_state;

    GtkBox      *box_header_title;

    GtkEventBox *event_box_close;
    GtkEventBox *event_box_info;

    GtkLabel    *lbl_title;
    GtkImage    *imgbtn_close;
    GtkImage    *imgbtn_info;
    GtkStack    *stack_subpage;

    GtkImage            *img_list;
    GtkLabel            *lbl_summary;
    GtkButton           *btn_download;
    GtkButton           *btn_open_directory;

    char directory[4096];
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcDetailRecvPage, ftc_detail_recv_page, GTK_TYPE_BOX)


////////////////////////    Private 선언   ////////////////////////
static void ftc_detail_recv_page_on_button_press_event_box_close(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_recv_page_on_button_press_event_box_info(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_recv_page_on_click_btn_download(GtkWidget *widget, gpointer data);

static void ftc_detail_recv_page_on_click_btn_open_directory(GtkWidget *widget, gpointer data);

static bool ftc_detail_recv_page_get_recv_file_list_and_info(FtcDetailRecvPage *page);

static void ftc_detail_recv_page_parse_select_pack_file_list(FtcDetailRecvPage *page, JsonArray *root_array);

static void ftc_detail_recv_page_parse_select_pack_file_request_info(FtcDetailRecvPage *page, JsonNode *node);

static bool ftc_detail_recv_page_file_download(FtcDetailRecvPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, GSList *file_list, RequestDownloadState *request_down_state);

static bool ftc_detail_recv_page_choice_directory(FtcDetailRecvPage *page);

static const char* ftc_detail_recv_page_get_save_directory(FtcDetailRecvPage *page);

static int ftc_detail_recv_page_get_index_of_request_file_info(FtcDetailRecvPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, const char *request_info_file_list_uid);

static bool ftc_detail_recv_page_confirm_overwrite_file_if_exist(FtcDetailRecvPage *page, const char *path);

static void ftc_detail_recv_page_update_start_downloading_state(FtcDetailRecvPage *page, RequestDownloadState *request_down_state, std::vector<std::string> &vc_file_uid);

static void ftc_detail_recv_page_update_activation_view(FtcDetailRecvPage *page);

static void ftc_detail_recv_page_download_result_event(Ftc::Core::Event *evt);

static void ftc_detail_recv_page_download_file_downloading_progress(FtcDetailRecvPage *page, JsonNode *node);

static void ftc_detail_recv_page_download_file_downloading_progress_callback(GtkWidget *widget, gpointer data);

static void ftc_detail_recv_page_download_file_complete(FtcDetailRecvPage *page, JsonNode *node);

static void ftc_detail_recv_page_download_finish(FtcDetailRecvPage *page, const char *request_info_uid, bool success, std::set<std::string>* error_list);
///////////////////////////////////////////////////////////////////

static void
ftc_detail_recv_page_init (FtcDetailRecvPage *page)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    const char *visible_page = NULL;
    Ftc::Core::ClientConfig client_config = Ftc::Core::GlobalVar::getClientConfig();
    Ftc::Core::EventManager *event_mgr = Ftc::Core::EventManager::getInstance();

    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private (page);

    //  Todo..
    memset(priv->directory, 0, 4096);

    //  subpage 생성
    //
    priv->filelist_sub_page = ftc_detail_recv_file_list_sub_page_new();
    gtk_stack_add_named(priv->stack_subpage, GTK_WIDGET(priv->filelist_sub_page), FTC_VIEW_DETAIL_RECV_SUB_PAGE_FILE_LIST);

    priv->requestinfo_sub_page = ftc_detail_recv_request_info_sub_page_new();
    gtk_stack_add_named(priv->stack_subpage, GTK_WIDGET(priv->requestinfo_sub_page), FTC_VIEW_DETAIL_RECV_SUB_PAGE_REQUEST_INFO);
    gtk_stack_set_visible_child_name(priv->stack_subpage, FTC_VIEW_DETAIL_RECV_SUB_PAGE_FILE_LIST);

    //  Signal (Event) 함수 연결
    //  event_box_close(imgbtn_close) click
    g_signal_connect(priv->event_box_close, "button-press-event", G_CALLBACK(ftc_detail_recv_page_on_button_press_event_box_close), page);

    //  event_box_info(imgbtn_info) click
    g_signal_connect(priv->event_box_info, "button-press-event", G_CALLBACK(ftc_detail_recv_page_on_button_press_event_box_info), page);

    //  btn_download click
    g_signal_connect(priv->btn_download, "clicked", G_CALLBACK(ftc_detail_recv_page_on_click_btn_download), page);

    //  btn_open_directory click
    g_signal_connect(priv->btn_open_directory, "clicked", G_CALLBACK(ftc_detail_recv_page_on_click_btn_open_directory), page);

    //  Widget 크기 설정
    //
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_header_title), 420, 45);
    gtk_widget_set_size_request(GTK_WIDGET(priv->btn_download), 114, 40);
    gtk_widget_set_size_request(GTK_WIDGET(priv->btn_open_directory), 114, 40);

    //  이미지 설정
    gtk_image_set_from_resource(priv->imgbtn_close, "/com/hunesion/ftcapp/CLOSE-ICON-16PX.png");
    gtk_image_set_from_resource(priv->img_list, "/com/hunesion/ftcapp/LIST02-ICON-16PX.png");

    visible_page = gtk_stack_get_visible_child_name(priv->stack_subpage);
    if (strcmp(visible_page, FTC_VIEW_DETAIL_RECV_SUB_PAGE_FILE_LIST) == 0) {
        gtk_image_set_from_resource(priv->imgbtn_info, "/com/hunesion/ftcapp/DETAIL-ICON-16PX.png");
    } else if (strcmp(visible_page, FTC_VIEW_DETAIL_RECV_SUB_PAGE_REQUEST_INFO) == 0) {
        gtk_image_set_from_resource(priv->imgbtn_info, "/com/hunesion/ftcapp/LIST03-ICON.png");
    }

    //  CSS 설정
    //  Header Background 색상 설정
    gtk_widget_set_name(GTK_WIDGET(page), "default_background_window");
    gtk_widget_set_name(GTK_WIDGET(priv->box_header_title), "box_detail_header_background");

    //  위젯설정
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_title), "label_detail_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_summary), "label_recv_new_count");
    //  버튼설정
    if (ftc_ui_is_internal_location()) {
        gtk_widget_set_name(GTK_WIDGET(priv->btn_download), "button_is");                
    } else {
        gtk_widget_set_name(GTK_WIDGET(priv->btn_download), "button_es");
    }
    gtk_widget_set_name(GTK_WIDGET(priv->btn_open_directory), "button_choice_directory");
    
    if (event_mgr != NULL) {
        event_mgr->addEventListener(page, FTC_CORE_DOWNLOAD_RECV_RESULT_EVENT, HUNE_CALLBACK_CLS_STATIC_1(ftc_detail_recv_page_download_result_event));
    }

    //  기본 저장 디렉토리로 사용자 다운로드 디렉토리로 설정한다.
    strcpy(priv->directory, client_config.getUserDownloadDirPath().c_str());
}

static void
ftc_detail_recv_page_dispose (GObject *object)
{
    FtcDetailRecvPagePrivate *priv;
    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(FTC_DETAIL_RECV_PAGE(object));

    priv->ftc_request = NULL;
    priv->request_down_state = NULL;
    Ftc::Core::EventManager::getInstance()->removeEventListener(object);
    
    G_OBJECT_CLASS(ftc_detail_recv_page_parent_class)->dispose(object);
}

static void
ftc_detail_recv_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_detail_recv_page_parent_class)->finalize(object);
}

static void
ftc_detail_recv_page_class_init (FtcDetailRecvPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_detail_recv_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_detail_recv_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-detail-recv-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, lbl_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, imgbtn_close);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, imgbtn_info);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, stack_subpage);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, box_header_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, event_box_close);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, event_box_info);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, img_list);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, lbl_summary);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, btn_download);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvPage, btn_open_directory);
}

FtcDetailRecvPage *
ftc_detail_recv_page_new (FtcHomeRecvListSubPage *recvlist_sub_page)
{
    FtcDetailRecvPage *page = FTC_DETAIL_RECV_PAGE(g_object_new (FTC_DETAIL_RECV_PAGE_TYPE, NULL));
    FtcDetailRecvPagePrivate *priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private (page);

    priv->recvlist_sub_page = recvlist_sub_page;

    return page;
}

static void 
ftc_detail_recv_page_on_button_press_event_box_close(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcDetailRecvPage *page = NULL;
    FtcDetailRecvPagePrivate *priv = NULL;
    gpointer main_window = NULL;
    GdkEventButton *event_button = (GdkEventButton*)event;

    if (! FTC_IS_DETAIL_RECV_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_RECV_PAGE(data);

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        if (ftc_ui_is_extension_widnow()) {
            ftc_ui_set_extension_window(main_window, false);
        }

        ftc_home_recv_list_sub_page_list_box_unselect_all(priv->recvlist_sub_page);
    }
}

static void
ftc_detail_recv_page_on_button_press_event_box_info(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcDetailRecvPage *page = NULL;
    FtcDetailRecvPagePrivate *priv = NULL;
    GdkEventButton *event_button = (GdkEventButton*)event;
    const char *visible_page = NULL;

    if (! FTC_IS_DETAIL_RECV_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_RECV_PAGE(data);

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        visible_page = gtk_stack_get_visible_child_name(priv->stack_subpage);
        if (strcmp(visible_page, FTC_VIEW_DETAIL_RECV_SUB_PAGE_FILE_LIST) == 0) {
            gtk_image_set_from_resource(priv->imgbtn_info, "/com/hunesion/ftcapp/LIST03-ICON.png");
            gtk_stack_set_visible_child_name(priv->stack_subpage, FTC_VIEW_DETAIL_RECV_SUB_PAGE_REQUEST_INFO);
        } else if (strcmp(visible_page, FTC_VIEW_DETAIL_RECV_SUB_PAGE_REQUEST_INFO) == 0) {
            gtk_image_set_from_resource(priv->imgbtn_info, "/com/hunesion/ftcapp/DETAIL-ICON-16PX.png");
            gtk_stack_set_visible_child_name(priv->stack_subpage, FTC_VIEW_DETAIL_RECV_SUB_PAGE_FILE_LIST);
        }
    }
}

static void 
ftc_detail_recv_page_on_click_btn_download(GtkWidget *widget, gpointer data)
{
    FtcDetailRecvPage *page = NULL;
    FtcDetailRecvPagePrivate *priv = NULL;
    GSList *file_uid_list = NULL;
    int list_count = 0;
    const char *visible_page = NULL;
    bool down_success = false;

    if (! FTC_IS_DETAIL_RECV_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_RECV_PAGE(data);

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (priv->request_down_state->is_downloading == true) {
        //  취소 처리
        Ftc::Core::Transfer::Download *download = Ftc::Core::Transfer::Download::getInstance();
        if (! download) {
            return;
        }

        if (download->cancel(priv->ftc_request->request_info_uid) == true) {
            //  취소 성공
            ftc_detail_recv_page_download_finish(page, priv->ftc_request->request_info_uid.c_str(), true, NULL);
        }
    } else {
        visible_page = gtk_stack_get_visible_child_name(priv->stack_subpage);
        if (strcmp(visible_page, FTC_VIEW_DETAIL_RECV_SUB_PAGE_FILE_LIST) == 0) {
            file_uid_list = ftc_detail_recv_file_list_sub_page_get_checked_file_uid(priv->filelist_sub_page);
            if (! file_uid_list) {
                return;
            }

            //  체크된 항목이 있는지 확인
            //
            list_count = g_slist_length(file_uid_list);
            if (list_count < 2/*데이터가 없어도 1이 리턴됨*/) {
                //  체크된 항목이 없음
                //
                FTC_LOG_DEBUG("다운로드 체크된 항목이 없음 !");
                return;
            }

            down_success = ftc_detail_recv_page_file_download(page, priv->ftc_request, file_uid_list, priv->request_down_state);
        } else if (strcmp(visible_page, FTC_VIEW_DETAIL_RECV_SUB_PAGE_REQUEST_INFO) == 0) {
            down_success = ftc_detail_recv_page_file_download_all(page);
        }
    }

    if (file_uid_list) {
        g_slist_free(file_uid_list);
        file_uid_list = NULL;
    }
}

static void 
ftc_detail_recv_page_on_click_btn_open_directory(GtkWidget *widget, gpointer data)
{
    FtcDetailRecvPage *page = NULL;
    FtcDetailRecvPagePrivate *priv = NULL; 
    std::stringstream ss;

    if (! FTC_IS_DETAIL_RECV_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_RECV_PAGE(data);
    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    ftc_open_directory(priv->directory);
}

void                    
ftc_detail_recv_page_set_data(FtcDetailRecvPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, RequestDownloadState *request_down_state)
{
    FtcDetailRecvPagePrivate *priv = NULL; 
    const char *visible_page = NULL;
    std::string title_show, str = "";

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return;
    }

    if (! ftc_request || ! request_down_state) {
        return;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    priv->ftc_request = ftc_request;
    priv->request_down_state = request_down_state;

    //  Summary 라벨에 텍스트 입력
    //
    str = "총 " + std::to_string(priv->ftc_request->file_cnt) + "개 파일, " + ftc_ui_get_file_size_format(priv->ftc_request->totalFileSize);
    gtk_label_set_text(priv->lbl_summary, str.c_str());

    //  타이틀 헤더에 타이틀을 입력한다.
    //
    title_show = ftc_ui_reduce_text_pixel(GTK_WIDGET(priv->lbl_title), ftc_request->request_title.c_str(), 330);
    gtk_label_set_text(priv->lbl_title, title_show.c_str());
    if (title_show != ftc_request->request_title) {
        gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_title), ftc_request->request_title.c_str());
    }

    //  파일 목록과 정보들을 가져온다.
    //
    if (!ftc_detail_recv_page_get_recv_file_list_and_info(page)) {
        return;
    }

    //  파일 리스트를 구성한다.
    //
    ftc_detail_recv_file_list_sub_page_set_data(priv->filelist_sub_page, priv->ftc_request, priv->request_down_state);

    if (priv->ftc_request->file_cnt > 0) {
        gtk_stack_set_visible_child_name(priv->stack_subpage, FTC_VIEW_DETAIL_RECV_SUB_PAGE_FILE_LIST);
        gtk_image_set_from_resource(priv->imgbtn_info, "/com/hunesion/ftcapp/DETAIL-ICON-16PX.png");
        gtk_widget_set_sensitive(GTK_WIDGET(priv->btn_download), true);
    } else {
        //  메세지 전송
        gtk_stack_set_visible_child_name(priv->stack_subpage, FTC_VIEW_DETAIL_RECV_SUB_PAGE_REQUEST_INFO);
        gtk_image_set_from_resource(priv->imgbtn_info, "/com/hunesion/ftcapp/LIST03-ICON.png");
        gtk_widget_set_sensitive(GTK_WIDGET(priv->btn_download), false);
    }

    ftc_detail_recv_request_info_sub_page_set_data(priv->requestinfo_sub_page, priv->ftc_request);

    //  다운로드 중 상태일 시 다운로드 중 화면으로 구성한다.
    ftc_detail_recv_page_update_activation_view(page);
}

static bool 
ftc_detail_recv_page_get_recv_file_list_and_info(FtcDetailRecvPage *page)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    JsonNode *root_node = NULL;
    JsonArray *arr = NULL;
    bool rv = false;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    auto resp_sel_pack = Ftc::Core::Request::requestSelectPackFile_ajax(priv->ftc_request->request_info_uid.c_str(), "receive");
    

    rv = true;
    return rv;
}

static void 
ftc_detail_recv_page_parse_select_pack_file_list(FtcDetailRecvPage *page, JsonArray *root_array)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    JsonObject *obj = NULL;
    const char *tmp = NULL;
    int arr_length = 0;
    bool request_down_state_pushback = false;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! root_array) {
        return;
    }

    //  기존에 있던 리스트는 삭제한다.
    //
    priv->ftc_request->vc_file_info.clear();
}

static void 
ftc_detail_recv_page_parse_select_pack_file_request_info(FtcDetailRecvPage *page, JsonNode *node)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    JsonObject *obj = NULL;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! node) {
        return;
    }
}

static bool 
ftc_detail_recv_page_file_download(FtcDetailRecvPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, GSList *file_list, RequestDownloadState *request_down_state)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    Ftc::Core::Transfer::Download *download = Ftc::Core::Transfer::Download::getInstance();
    std::shared_ptr<Ftc::Core::Transfer::DownloadRequest> down_request(nullptr);
    std::vector<std::string> vc_file_uid;
    int checked_size = 0, file_uid_index = -1;
    const char *checked_uid = NULL;
    bool rv = false;
    Ftc::Core::FileInfo fi;
    std::string download_expire_dt, directory, list_type = "receive";

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }
    if (! ftc_request) {
        return rv;
    }
    if (! file_list) {
        return rv;
    }
    if (! request_down_state) {
        return rv;
    }

    //  유효기간을 체크한다.
    //
    download_expire_dt = ftc_request->download_expire_dt;
    if (ftc_ui_is_expired_date_request_info(download_expire_dt)) {
        return rv;
    }  

    //  디렉토리가 설정되어 있는지 검사하고 설정되어 있지 않다면 디렉토리 설정 창을 오픈한다.
    //
    fi = Ftc::Core::FileInfo::getFileInfo(ftc_detail_recv_page_get_save_directory(page));
    if (!fi.isExist()) {
        if (ftc_detail_recv_page_choice_directory(page) == false) {
            return rv;
        }

        fi = Ftc::Core::FileInfo::getFileInfo(ftc_detail_recv_page_get_save_directory(page));
        if (!fi.isExist()) {
            return rv;
        }
    }

    directory = fi.getFullPath();
    down_request = std::make_shared<Ftc::Core::Transfer::DownloadRequest>(ftc_request->request_info_uid, directory);

    checked_size = g_slist_length(file_list);
    for (int j = 1; j < checked_size; j++) {
        Ftc::Core::GlobalStruct::FtcRequestInfoFile file_info;
        std::shared_ptr<Ftc::Core::Transfer::DownloadFile> down_file(nullptr);
        std::string filename, fullpath, relative_dir;

        checked_uid = (const char*)g_slist_nth_data(file_list, j);
        if (!checked_uid) {
            continue;
        }
        
        //  uid를 기준으로 파일정보를 가져온다.
        //
        file_uid_index = ftc_detail_recv_page_get_index_of_request_file_info(page, ftc_request, checked_uid);
        if (file_uid_index < 0) {
            continue;
        }

        file_info = ftc_request->vc_file_info[file_uid_index];

        //  다운로드 가능한 파일인지 체크한다.
        //  다운로드 카운트 체크
        if (file_info.download_cnt >= file_info.download_limit_cnt) {
            continue;
        }

        filename = file_info.file_name;
        relative_dir = file_info.relative_dir;

        //  이미 해당 파일명이 다운로드 할 경로에 있는지 체크
        //
        fullpath = directory + "/";
        if (relative_dir.length() > 0) {
            fullpath += relative_dir + "/";
        }
        fullpath += filename;
        if (ftc_detail_recv_page_confirm_overwrite_file_if_exist(page, fullpath.c_str()) == true) {
            //  다운로드 파라미터 구성
            //
            down_file = std::make_shared<Ftc::Core::Transfer::DownloadFile>(down_request, file_info.request_info_file_list_uid,
                                                                        filename, ftc_request->from_network_id, ftc_request->to_network_id, list_type);
            //  폴더 구조 업로드일 경우
            if (relative_dir.length() > 0) {
                down_file->setRelativeDir(relative_dir);
            }
            
            down_request->pushDownloadFile(down_file);    
            vc_file_uid.push_back(file_info.request_info_file_list_uid);
        }
    }

    //  다운로드할 파일이 있으면 다운로드를 시작한다.
    //
    if (vc_file_uid.size() > 0) {
        down_request->setEvent(FTC_CORE_DOWNLOAD_RECV_RESULT_EVENT, page);
        download->pushRequest(down_request);

        //  다운로드 상태로 값 변경
        ftc_detail_recv_page_update_start_downloading_state(page, request_down_state, vc_file_uid);
        //  HOME RECV LIST의 다운로드 버튼을 전체 프로그레스로 변경
        ftc_home_recv_list_sub_page_set_visible_progress(priv->recvlist_sub_page, ftc_request->request_info_uid.c_str(), true, vc_file_uid.size());
        //  현재 다운로드 화면이 열려있으면 다운로드 화면으로 갱신한다.
        if (priv->ftc_request->request_info_uid == ftc_request->request_info_uid) {
            ftc_detail_recv_page_update_activation_view(page);
        }
        
        rv = true;
    }

    return rv;
}

bool 
ftc_detail_recv_page_file_download_all(FtcDetailRecvPage *page)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    GSList *file_uid_list = NULL;
    int list_count = 0;
    bool rv = false;
    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return rv;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    //  전체파일을 체크(선택)한다.
    //
    ftc_detail_recv_file_list_sub_page_set_all_check_active(priv->filelist_sub_page, true);

    file_uid_list = ftc_detail_recv_file_list_sub_page_get_checked_file_uid(priv->filelist_sub_page);
    if (! file_uid_list) {
        return rv;
    }

    //  체크된 항목이 있는지 확인
    //
    list_count = g_slist_length(file_uid_list);
    if (list_count < 2/*데이터가 없어도 1이 리턴됨*/) {
        //  체크된 항목이 없음
        //
        FTC_LOG_DEBUG("다운로드 가능한 파일이 없음 !");
        ftc_ui_message_box(GtkMessageType::GTK_MESSAGE_INFO, GtkButtonsType::GTK_BUTTONS_OK, "다운로드 허용 횟수를 초과하였습니다.");
        return rv;
    }

    rv = ftc_detail_recv_page_file_download(page, priv->ftc_request, file_uid_list, priv->request_down_state);

    if (file_uid_list) {
        g_slist_free(file_uid_list);
        file_uid_list = NULL;
    }

    return rv;
}

static bool 
ftc_detail_recv_page_choice_directory(FtcDetailRecvPage *page)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    GtkWidget *file_chooser_dlg = NULL;
    GtkFileChooser *file_chooser = NULL;
    bool rv = false;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    file_chooser_dlg = gtk_file_chooser_dialog_new("폴더 선택", GTK_WINDOW(ftc_ui_get_main_window()), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                        "취소", GTK_RESPONSE_CANCEL, "열기", GTK_RESPONSE_ACCEPT, NULL);
    if (file_chooser_dlg == NULL) {
        return rv;
    }

    file_chooser = GTK_FILE_CHOOSER(file_chooser_dlg);
    if (! file_chooser) {
        return rv;
    }

    if(gtk_dialog_run(GTK_DIALOG(file_chooser_dlg)) == GTK_RESPONSE_ACCEPT) {
        strcpy(priv->directory, gtk_file_chooser_get_filename(file_chooser));
        rv = true;
    }

    gtk_widget_destroy(GTK_WIDGET(file_chooser_dlg));

    return rv;
}

static const char* 
ftc_detail_recv_page_get_save_directory(FtcDetailRecvPage *page)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    const char *rv = NULL;
    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private (page);
    if (! priv) {
        return rv;
    }
    rv = priv->directory;
    return rv;
}

static int
ftc_detail_recv_page_get_index_of_request_file_info(FtcDetailRecvPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, const char *request_info_file_list_uid)
{
    bool find = false;
    int rv = -1, index = 0;

    if (! ftc_request) {
        return rv;
    }
    if (! request_info_file_list_uid) {
        return rv;
    }

    for (auto &it : ftc_request->vc_file_info) {
        if (it.request_info_file_list_uid == request_info_file_list_uid) {
            find = true;
            break;
        }

        index++;
    }

    rv = find ? index : -1;
    return rv;
}

static bool 
ftc_detail_recv_page_confirm_overwrite_file_if_exist(FtcDetailRecvPage *page, const char *path)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    GFile *file = NULL;
    std::string message;
    int dialog_result = 0;
    bool rv = false;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return rv;
    }
    if (! path) {
        return rv;
    }

    file = g_file_new_for_path(path);
    if (! file) {
        return rv;
    }

    if (g_file_query_exists(file, NULL) == false) {
        rv = true;
        return rv;
    }

    message.append(path);
    message.append("파일이 이미 있습니다.\n 덮어쓰시겠습니까?");

    //  파일이 이미 있음 덮어쓸지 여부 확인
    dialog_result = ftc_ui_message_box(GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL, message.c_str());

    if (dialog_result == GTK_RESPONSE_OK) {
        rv = true;
    }

    return rv;
}

static void
ftc_detail_recv_page_update_start_downloading_state(FtcDetailRecvPage *page, RequestDownloadState *request_down_state, std::vector<std::string> &vc_file_uid)
{
    FtcDetailRecvPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! request_down_state) {
        return;
    }

    if (vc_file_uid.size() <= 0) {
        return;
    }

    request_down_state->is_downloading = true;
    
    for (auto &state_uid : request_down_state->vc_file_down_status) {
        for (auto &file_uid : vc_file_uid) {
            if (state_uid.request_info_file_list_uid == file_uid) {
                state_uid.is_downloading = true;
                break;
            }
        }
    }
}

static void
ftc_detail_recv_page_update_activation_view(FtcDetailRecvPage *page)
{
    FtcDetailRecvPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (priv->request_down_state->is_downloading == true) {
        //  다운로드 중(or 시작)
        gtk_button_set_label(priv->btn_download, "취소");
    } else {
        //  다운로드 종료(or 다운로드 중이 아님)
        gtk_button_set_label(priv->btn_download, "받기");
    }

    //  개별 프로그레스
    ftc_detail_recv_file_list_sub_page_update_activation_view(priv->filelist_sub_page);
}


static void 
ftc_detail_recv_page_download_result_event(Ftc::Core::Event *evt)
{
    FtcDetailRecvPage *page = NULL;
    FtcDetailRecvPagePrivate *priv = NULL;
    Ftc::Core::Transfer::DownloadResultParam *param = NULL;
    
    if (! evt) {
        return;
    }
    if (! evt->getUserData()){
        return;
    }

    param = (Ftc::Core::Transfer::DownloadResultParam*)evt->getUserData();

    if (! FTC_IS_DETAIL_RECV_PAGE(param->user_data)) {
        return;
    }
    page = FTC_DETAIL_RECV_PAGE((gpointer)param->user_data);

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv){
        return;
    }

    if (! param->node) {
        return;
    }

    switch (param->status)
    {
    case Ftc::Core::Transfer::DownloadStatus::DOWNLOADING :
        {
            //  프로그레스 갱신
            ftc_detail_recv_page_download_file_downloading_progress(page, param->node);
        }
        break;
    case Ftc::Core::Transfer::DownloadStatus::DOWNLOAD_COMPLETE :
        {
            //  다운로드 건 완료 처리
            ftc_detail_recv_page_download_file_complete(page, param->node);
        }
        break;
    default:
        break;
    }
}

static void
ftc_detail_recv_page_download_file_downloading_progress(FtcDetailRecvPage *page, JsonNode *node)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    std::string request_info_uid, request_info_file_list_uid;
    int progress = 0;
    bool viewing = false;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! node) {
        return;
    }

    request_info_uid = Ftc::Core::JsonUtils::getStdString(node, "info:request_info_uid");
    if (request_info_uid.length() <= 0) {
        return;
    }
    request_info_file_list_uid = Ftc::Core::JsonUtils::getStdString(node, "info:request_info_file_list_uid");
    if (request_info_file_list_uid.length() <= 0) {
        return;
    }
    progress = Ftc::Core::JsonUtils::getInt(node, "info:progress");

    if (priv->ftc_request) {
        if (priv->ftc_request->request_info_uid == request_info_uid) {
            //  현재 화면을 갱신한다.
            ftc_detail_recv_file_list_sub_page_set_progress_value(priv->filelist_sub_page, request_info_file_list_uid.c_str(), progress);

            viewing = true;
        }
    }

    if (viewing == false) {
        //  프로그레스 데이터를 RecvList로 넘긴다.
        ftc_home_recv_list_sub_page_set_progress_background_value(priv->recvlist_sub_page, request_info_uid.c_str(), request_info_file_list_uid.c_str(), progress);
    }

    if (progress >= 100) {
        //  전체 프로그레스 갱신
        ftc_home_recv_list_sub_page_update_total_progress(priv->recvlist_sub_page, request_info_uid.c_str());
    }
}

static void
ftc_detail_recv_page_download_file_complete(FtcDetailRecvPage *page, JsonNode *node)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    JsonNode *jelement = NULL;
    JsonArray *jarr = NULL;
    std::set<std::string> error_list;
    std::string request_info_uid, error_msg;
    int arr_size = 0;
    bool success = false;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! node) {
        return;
    }

    request_info_uid = Ftc::Core::JsonUtils::getStdString(node, "info:request_info_uid");
    if (request_info_uid.length() <= 0) {
        return;
    }
    if (Ftc::Core::JsonUtils::getStdString(node, "info:result") == "success") {
        success = true;
    } else {
        success = false;

        jarr = Ftc::Core::JsonUtils::getArray(node, "info:fail_list");
        if (jarr) {
            arr_size = json_array_get_length(jarr);
            for (int i = 0; i < arr_size; i++) {
                jelement = json_array_get_element(jarr, i);
                if (! jelement) {
                    continue;
                }

                error_msg = Ftc::Core::JsonUtils::getStdString(jelement, "error_msg");
                if (error_msg.length() > 0) {
                    error_list.insert(error_msg);
                }
            }
        }
    }

    ftc_detail_recv_page_download_finish(page, request_info_uid.c_str(), success, &error_list);
}

static void
ftc_detail_recv_page_download_finish(FtcDetailRecvPage *page, const char *request_info_uid, bool success, std::set<std::string>* error_list)
{
    FtcDetailRecvPagePrivate *priv = NULL;
    std::string error_msg;
    bool first = true;

    if (! FTC_IS_DETAIL_RECV_PAGE(page)) {
        return;
    }

    priv = (FtcDetailRecvPagePrivate*)ftc_detail_recv_page_get_instance_private(page);
    if (! priv) {
        return;
    }
    if (! request_info_uid) {
        return;
    }

    ftc_home_recv_list_sub_page_download_complete(priv->recvlist_sub_page, request_info_uid);
    ftc_home_recv_list_sub_page_set_visible_progress(priv->recvlist_sub_page, request_info_uid, false, 0);

    if (priv->ftc_request) {
        if (priv->ftc_request->request_info_uid == request_info_uid) {
            ftc_detail_recv_page_get_recv_file_list_and_info(page);
            ftc_detail_recv_file_list_sub_page_set_data(priv->filelist_sub_page, priv->ftc_request, priv->request_down_state);
            ftc_detail_recv_page_update_activation_view(page);
        }
    }

    if (! success && error_list) {
        error_msg = "아래와 같은 사유로 다운로드가 불가합니다.\n\n";
        for (auto it : *error_list) {
            error_msg.append(it);
            if (first) {
                first = false;
            } else {
                error_msg.append("\n");
            }
        }

        ftc_ui_message_box(GtkMessageType::GTK_MESSAGE_ERROR, GtkButtonsType::GTK_BUTTONS_OK, error_msg.c_str());
    }
}   