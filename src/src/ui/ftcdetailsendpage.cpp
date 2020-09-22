#include "ftcdetailsendpage.h"
#include "../core/StringUtils.h"
#include <sstream>
#include <vector>


enum HEADER_SUB_TITLE
{
    TARGET_NETWORK,
    RECEIVER_USER,
    SEND_REASON
};

typedef struct _NetworkData
{
    std::string direction;
    std::string network_direction_id;
    std::string from_network_id;
    std::string to_name;
    std::string to_network_id;
} NetworkData;

typedef struct _NetworkFindParam
{
    const char *network_id;
    const char *network_name;
    const NetworkData *network_data;
} NetworkFindParam;

typedef struct _GetCheckNetworkParam
{
    FtcDetailSendPage *page;
    std::vector<Ftc::Core::GlobalStruct::FtcNetwork> vc_network_list;
} GetCheckNetworkParam;

struct _FtcDetailSendPage
{
    GtkBox parent;
};

typedef struct _FtcDetailSendPagePrivate FtcDetailSendPagePrivate;

struct _FtcDetailSendPagePrivate
{
    FtcHomeSendListSubPage   *sendlist_sub_page;

    GtkBox          *box_header_title;
    GtkLabel        *lbl_title_send_option;
    GtkImage        *imgbtn_close;
    GtkBox          *box_option_area;
    GtkBox          *box_target_network;
    GtkBox          *box_header_target_network;
    GtkBox          *box_target_network_list;
    GtkLabel        *lbl_title_target_network;
    GtkLabel        *lbl_title_target_network_detail;
    GtkBox          *box_target_network_area;
    GtkImage        *imgbtn_target_network_option;
    GtkBox          *box_receiver;
    GtkBox          *box_header_receiver;
    GtkLabel        *lbl_title_receiver;
    GtkLabel        *lbl_title_receiver_detail;
    GtkBox          *box_receiver_area;
    GtkBox          *box_radiobtn_area;
    GtkRadioButton  *rdbtn_send_to_me;
    GtkRadioButton  *rdbtn_send_to_other;
    GtkImage        *imgbtn_select_target_user;
    GtkBox          *box_send_reason;
    GtkBox          *box_header_send_reason;
    GtkLabel        *lbl_title_send_reason;
    GtkBox          *box_send_reason_area;
    GtkTextView     *txtview_send_reason;

    GtkEventBox     *event_box_close;
    GtkEventBox     *event_box_header_target_network;
    GtkEventBox     *event_box_target_network_option;
    GtkEventBox     *event_box_header_receiver;
    GtkEventBox     *event_box_select_target_user;
    GtkEventBox     *event_box_header_send_reason;

    GtkScrolledWindow *scrolled_window1;

    GSList *network_list;
    Ftc::Core::UploadOption     *upload_option;
    Ftc::Core::PackFilePop      *pack_file_pop;
    bool            is_approval_update;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcDetailSendPage, ftc_detail_send_page, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_detail_send_page_on_button_press_event_box_close(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_send_page_on_button_press_event_box_target_network_option(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_send_page_on_button_press_event_box_header_receiver(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_send_page_on_button_press_event_box_select_target_user(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_send_page_on_button_press_event_box_header_send_reason(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_detail_send_page_on_toggled_network_checkbox(GtkToggleButton *toggle_button, gpointer data);

static void ftc_detail_send_page_on_toggled_rdbtn_send_to_me(GtkToggleButton *toggle_button, gpointer data);

static void ftc_detail_send_page_on_toggled_rdbtn_send_to_other(GtkToggleButton *toggle_button, gpointer data);

static void ftc_detail_send_page_open_approval_web_view(FtcDetailSendPage *page, std::vector<Ftc::Core::GlobalStruct::FtcNetwork> checked_network, Ftc::Core::UploadOption *upload_option, bool is_auto);

static void ftc_detail_send_page_get_target_network_list(FtcDetailSendPage *page);

static void ftc_detail_send_page_add_target_network(FtcDetailSendPage *page, const char *network_name);

static void ftc_detail_send_page_get_selection_target_network(FtcDetailSendPage *page);

static void ftc_detail_send_page_set_network_check(FtcDetailSendPage *page, const char *network_name);

static void ftc_detail_send_page_set_network_check_callback(GtkWidget *widget, gpointer data);

static void ftc_detail_send_page_set_network_uncheck_all(FtcDetailSendPage *page);

static void ftc_detail_send_page_set_network_uncheck_all_callback(GtkWidget *widget, gpointer data);

static std::vector<Ftc::Core::GlobalStruct::FtcNetwork> ftc_detail_send_page_add_upload_option_checked_network(FtcDetailSendPage *page);

static void ftc_detail_send_page_add_upload_option_checked_network_callback(GtkWidget *widget, gpointer data);

static void ftc_detail_send_page_delete_all_list_network_data(FtcDetailSendPage *page);

static void ftc_detail_send_page_delete_all_list_network_data_callback(gpointer data, gpointer user_data);

static const NetworkData* ftc_detail_send_page_find_network_data_from_id(FtcDetailSendPage *page, const char *network_id);

static void ftc_detail_send_page_find_network_data_from_id_callback(gpointer data, gpointer user_data);

static Ftc::Core::GlobalStruct::FtcNetwork ftc_detail_send_page_find_ftc_network_from_network_name(FtcDetailSendPage *page, const char *network_name);

static void ftc_detail_send_page_select_receive_user_webview_event(Ftc::Core::Event *evt);

static void ftc_detail_send_page_parse_user_select_values(FtcDetailSendPage *page, JsonNode *node);

static void ftc_detail_send_page_set_receiver_detail(FtcDetailSendPage *page);

static void ftc_detail_send_page_approval_line_config_webview_event(Ftc::Core::Event *evt);

static std::string ftc_detail_send_page_get_check_network_list_string(FtcDetailSendPage *page);

static void ftc_detail_send_page_get_check_network_list_string_callback(GtkWidget *widget, gpointer data);
///////////////////////////////////////////////////////////////////

static void
ftc_detail_send_page_init (FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = Ftc::Core::EventManager::getInstance();
    const Ftc::Core::Properties &properties = Ftc::Core::GlobalVar::getProperties();

    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private (page);

    priv->upload_option = new UploadOption();
    priv->network_list = g_slist_alloc();

    //  Signal (Event) 함수 연결
    //  imgbtn_close(event_box_close) click
    g_signal_connect(priv->event_box_close, "button-press-event", G_CALLBACK(ftc_detail_send_page_on_button_press_event_box_close), page);

    //  imgbtn_target_network_option(event_box_target_network_option) click
    g_signal_connect(priv->event_box_target_network_option, "button-press-event", G_CALLBACK(ftc_detail_send_page_on_button_press_event_box_target_network_option), page);

    //  imgbtn_select_target_user(event_box_select_target_user) click
    g_signal_connect(priv->event_box_select_target_user, "button-press-event", G_CALLBACK(ftc_detail_send_page_on_button_press_event_box_select_target_user), page);
    
    //  rdbtn_send_to_me
    g_signal_connect(priv->rdbtn_send_to_me, "toggled", G_CALLBACK(ftc_detail_send_page_on_toggled_rdbtn_send_to_me), page);

    //  rdbtn_send_to_other
    g_signal_connect(priv->rdbtn_send_to_other, "toggled", G_CALLBACK(ftc_detail_send_page_on_toggled_rdbtn_send_to_other), page);

    //  버튼 이미지 설정
    gtk_image_set_from_resource(priv->imgbtn_close, "/com/hunesion/ftcapp/CLOSE-ICON-16PX.png");
    gtk_image_set_from_resource(priv->imgbtn_target_network_option, "/com/hunesion/ftcapp/APPROVAL-ICON-16PX.png");
    gtk_image_set_from_resource(priv->imgbtn_select_target_user, "/com/hunesion/ftcapp/USERCHECK-ICON-16PX.png");

    //  WIDGET 사이즈 설정
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_header_title), 420, 45);
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_target_network_area), 400, 97);
    gtk_widget_set_size_request(GTK_WIDGET(priv->event_box_header_receiver), 400, 30);
    gtk_widget_set_size_request(GTK_WIDGET(priv->event_box_header_send_reason), 400, 30);
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_send_reason_area), 400, 206);
    gtk_widget_set_size_request(GTK_WIDGET(priv->txtview_send_reason), 370, 176);
    

    //  CSS 설정
    //  Background 색상 설정
    gtk_widget_set_name(GTK_WIDGET(priv->box_header_title), "box_detail_header_background");
    gtk_widget_set_name(GTK_WIDGET(priv->box_option_area), "default_background_window");
    gtk_widget_set_name(GTK_WIDGET(priv->event_box_header_target_network), "box_detail_header_sub_title");
    gtk_widget_set_name(GTK_WIDGET(priv->box_target_network_area), "default_background_page");
    gtk_widget_set_name(GTK_WIDGET(priv->event_box_header_receiver), "box_detail_header_sub_title");
    gtk_widget_set_name(GTK_WIDGET(priv->box_receiver_area), "default_background_page");
    gtk_widget_set_name(GTK_WIDGET(priv->event_box_header_send_reason), "box_detail_header_sub_title");
    gtk_widget_set_name(GTK_WIDGET(priv->box_send_reason_area), "default_background_page");

    //  widget 설정
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_title_send_option), "label_detail_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_title_target_network), "label_send_detail_sub_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_title_target_network_detail), "label_send_detail_sub_title_detail");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_title_receiver), "label_send_detail_sub_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_title_receiver_detail), "label_send_detail_sub_title_detail");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_title_send_reason), "label_send_detail_sub_title");

    gtk_widget_set_name(GTK_WIDGET(priv->rdbtn_send_to_me), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->rdbtn_send_to_other), "label_recv_info_title");

    gtk_widget_set_name(GTK_WIDGET(priv->txtview_send_reason), "entry_default");

    if (event_mgr) {
        event_mgr->addEventListener(page, FTC_UI_SELECT_RECEIVE_USER, HUNE_CALLBACK_CLS_STATIC_1(ftc_detail_send_page_select_receive_user_webview_event));
        event_mgr->addEventListener(page, FTC_UI_SELECT_INIT_PACK_FILE_POP_CONFIG, HUNE_CALLBACK_CLS_STATIC_1(ftc_detail_send_page_approval_line_config_webview_event));
    }

    ftc_detail_send_page_get_target_network_list(page);

    priv->is_approval_update = false;
}

static void
ftc_detail_send_page_dispose (GObject *object)
{
    FtcDetailSendPagePrivate *priv;
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(FTC_DETAIL_SEND_PAGE(object));

    if (priv->upload_option) {
        delete priv->upload_option;
        priv->upload_option = NULL;
    }

    if (priv->network_list) {
        ftc_detail_send_page_delete_all_list_network_data(FTC_DETAIL_SEND_PAGE(object));
        g_slist_free(priv->network_list);
        priv->network_list = NULL;
    }

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);

    priv->pack_file_pop = NULL;
    
    G_OBJECT_CLASS(ftc_detail_send_page_parent_class)->dispose(object);
}

static void
ftc_detail_send_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_detail_send_page_parent_class)->finalize(object);
}

static void
ftc_detail_send_page_class_init (FtcDetailSendPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_detail_send_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_detail_send_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-detail-send-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_header_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, lbl_title_send_option);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, imgbtn_close);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_option_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_target_network);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_header_target_network);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, lbl_title_target_network);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, lbl_title_target_network_detail);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_target_network_list);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_target_network_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, imgbtn_target_network_option);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_receiver);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_header_receiver);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, lbl_title_receiver);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, lbl_title_receiver_detail);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_receiver_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_radiobtn_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, rdbtn_send_to_me);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, rdbtn_send_to_other);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, imgbtn_select_target_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_send_reason);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_header_send_reason);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, lbl_title_send_reason);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, box_send_reason_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, txtview_send_reason);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, event_box_close);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, event_box_header_target_network);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, event_box_target_network_option);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, event_box_header_receiver);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, event_box_select_target_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, event_box_header_send_reason);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailSendPage, scrolled_window1);
}

FtcDetailSendPage *
ftc_detail_send_page_new (FtcHomeSendListSubPage *sendlist_sub_page)
{
    FtcDetailSendPage *page = FTC_DETAIL_SEND_PAGE(g_object_new (FTC_DETAIL_SEND_PAGE_TYPE, NULL));
    FtcDetailSendPagePrivate *priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private (page);

    priv->sendlist_sub_page = sendlist_sub_page;

    return page;
}

void                    
ftc_detail_send_page_enable(FtcDetailSendPage *page, Ftc::Core::PackFilePop *pack_file_pop)
{
    FtcDetailSendPagePrivate *priv = NULL;
    GtkTextBuffer *txt_buf = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
     
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    if (! pack_file_pop) {
        return;
    }

    priv->pack_file_pop = pack_file_pop;

    //  네트워크 기본 체크박스 선택
    //
    ftc_detail_send_page_get_selection_target_network(page);

    //  전송사유에 기본 텍스트(포멧)가 있으면 입력해준다.
    txt_buf = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(txt_buf, priv->pack_file_pop->getFtcContents().c_str(), priv->pack_file_pop->getFtcContents().length());
    gtk_text_view_set_buffer(priv->txtview_send_reason, txt_buf);
}


void                    
ftc_detail_send_page_disable(FtcDetailSendPage *page)
{  
    GtkTextBuffer *txt_buf = NULL;
    FtcDetailSendPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }

    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    //  네트워크 망 체크박스 해제
    //
    ftc_detail_send_page_set_network_uncheck_all(page);

    priv->upload_option->clearNetwork();
    priv->upload_option->clearUser();
    priv->upload_option->clearSendReason();
    priv->upload_option->clearApprovalLines();
    priv->pack_file_pop = NULL;
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->rdbtn_send_to_other)) == true) {
        gtk_label_set_text(priv->lbl_title_receiver_detail, "");
    }

    txt_buf = gtk_text_buffer_new(NULL);
    gtk_text_view_set_buffer(priv->txtview_send_reason, txt_buf);
}

static void 
ftc_detail_send_page_on_button_press_event_box_close(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcDetailSendPage *page = NULL;
    FtcDetailSendPagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(data);
    
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        ftc_home_send_list_sub_page_activate_extension_window(priv->sendlist_sub_page, false);
    }
}

static void 
ftc_detail_send_page_on_button_press_event_box_target_network_option(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcDetailSendPage *page = NULL;
    FtcDetailSendPagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(data);
    
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        if (ftc_home_send_list_sub_page_is_uploading(priv->sendlist_sub_page) == false) {
            ftc_detail_send_page_open_approval_web_view(page, ftc_detail_send_page_add_upload_option_checked_network(page), priv->upload_option, false);
        }
    }
}

static void                    
ftc_detail_send_page_open_approval_web_view(FtcDetailSendPage *page, std::vector<Ftc::Core::GlobalStruct::FtcNetwork> checked_network, Ftc::Core::UploadOption *upload_option, bool is_auto)
{
    FtcDetailSendPagePrivate *priv = NULL;
    std::string select_charge_info;
    JsonObject *json = NULL;
    JsonArray *jarr = NULL;
    HTTP_PARAM param;
    bool is_hide = false;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! upload_option) {
        return;
    }
    if (checked_network.size() <= 0) {
        return;
    }

    ftc_web_kit_show_dialog_post(GTK_WINDOW(ftc_ui_get_main_window()),  "WEB_KIT_URL_INIT_PACK_FILE_POP", &param, "결재선 선택", 800, 400,
                                    FTC_UI_SELECT_INIT_PACK_FILE_POP_CONFIG, page, true); 
}

static void 
ftc_detail_send_page_on_button_press_event_box_select_target_user(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcDetailSendPage *page = NULL;
    FtcDetailSendPagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    gpointer main_window = NULL;
    HTTP_PARAM param;

    if (! FTC_IS_DETAIL_SEND_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(data);

    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        if (ftc_home_send_list_sub_page_is_uploading(priv->sendlist_sub_page) == false) {
            ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window),  "WEB_KIT_URL_INIT_USER_GROUP_DIV_POP", &param, "사용자 선택", 800, 560,
                                        FTC_UI_SELECT_RECEIVE_USER, page); 
        }
    }
}

static void 
ftc_detail_send_page_get_target_network_list(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    JsonNode *node = NULL;
    JsonArray *list = NULL;
    JsonObject *obj = NULL;
    int list_count = 0;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    //  네트워크 리스트 가져오기
    //
    auto respNetworkList = Ftc::Core::Request::requestSelectEnableSendNetworkList();
    if (! respNetworkList->isSuccess()) {
        return;
    }

    for (int i = 0; i < list_count; i++) {
        NetworkData *network_data = new NetworkData();
        obj = json_array_get_object_element(list, i);
        
        network_data->direction = Ftc::Core::JsonUtils::getStdString(obj, "direction");
        network_data->from_network_id = Ftc::Core::JsonUtils::getStdString(obj, "from_network_id");
        network_data->network_direction_id = Ftc::Core::JsonUtils::getStdString(obj, "network_direction_id");
        network_data->to_name = Ftc::Core::JsonUtils::getStdString(obj, "to_name");
        network_data->to_network_id = Ftc::Core::JsonUtils::getStdString(obj, "to_network_id");
        priv->network_list = g_slist_insert(priv->network_list, network_data, -1);

        ftc_detail_send_page_add_target_network(page, network_data->to_name.c_str());

        //  대상망 체크박스 공간 크기 조절
        //  망이 6개가 넘어가면 스크롤로 전환시킨다.
        if (i == 0) {
            gtk_widget_set_size_request(GTK_WIDGET(priv->scrolled_window1), -1, 52);
            gtk_widget_set_size_request(GTK_WIDGET(priv->box_target_network_area), -1, 52);
        } else if (i < 6) {
            int height = (30 * (i + 1)) + 20;
            gtk_widget_set_size_request(GTK_WIDGET(priv->scrolled_window1), -1, height);
            gtk_widget_set_size_request(GTK_WIDGET(priv->box_target_network_area), -1, height);
        }
        
    }
}

static void 
ftc_detail_send_page_add_target_network(FtcDetailSendPage *page, const char *network_name)
{
    FtcDetailSendPagePrivate *priv = NULL;
    GtkCheckButton *checkbtn = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }
    if (! network_name) {
        return;
    }

    checkbtn = GTK_CHECK_BUTTON(gtk_check_button_new());
    if (! checkbtn) {
        return;
    }

    gtk_widget_set_name(GTK_WIDGET(checkbtn), "checkbox_network_name");
    gtk_button_set_label(GTK_BUTTON(checkbtn), network_name);
    gtk_widget_set_margin_top(GTK_WIDGET(checkbtn), 7);
    gtk_widget_show(GTK_WIDGET(checkbtn));
    gtk_box_pack_start(priv->box_target_network_list, GTK_WIDGET(checkbtn), false, false, 0);

    //  check box 클릭 이벤트 연결
    g_signal_connect(GTK_WIDGET(checkbtn), "toggled", G_CALLBACK(ftc_detail_send_page_on_toggled_network_checkbox), page);
}

static void 
ftc_detail_send_page_get_selection_target_network(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    JsonObject *obj = NULL;
    JsonNode *node = NULL;
    JsonArray *list = NULL;
    const char *selection_network_id = NULL;
    const NetworkData *network_data = NULL;
    int index = 0, list_count = 0;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    //  네트워크 기본 선택 가져오기 
    //
    auto respSelectionNetwork = Ftc::Core::Request::requestSelectNetworkSelectionUserList(Ftc::Core::GlobalVar::getLoginLocation().getNetworkId().c_str(),
                                                                                Ftc::Core::GlobalVar::getUser().getUserId().c_str());

    if (! respSelectionNetwork->isSuccess()) {
        return;
    }
}

static void 
ftc_detail_send_page_on_toggled_network_checkbox(GtkToggleButton *toggle_button, gpointer data)
{
    FtcDetailSendPage *page = NULL;
    FtcDetailSendPagePrivate *priv = NULL;
    Ftc::Core::GlobalStruct::FtcNetwork network;
    std::string target_network_detail, target_network_detail_show;
    const char *name = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(data);

    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    name = gtk_button_get_label(GTK_BUTTON(toggle_button));

    network =  ftc_detail_send_page_find_ftc_network_from_network_name(page, name);
    if (gtk_toggle_button_get_active(toggle_button) == true) {
        priv->pack_file_pop->putRecvNetwork(network.to_network_id.c_str());
    } else {
        priv->pack_file_pop->removeRecvNetwork(network.to_network_id.c_str());
    }

    target_network_detail = ftc_detail_send_page_get_check_network_list_string(page);
    target_network_detail_show = ftc_ui_reduce_text_pixel(GTK_WIDGET(priv->lbl_title_target_network_detail), target_network_detail.c_str(), 300);
    if (target_network_detail != target_network_detail_show) {
        gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_title_target_network_detail), target_network_detail.c_str());
    }
    gtk_label_set_text(priv->lbl_title_target_network_detail, target_network_detail_show.c_str());

    priv->is_approval_update = false;
}

static void 
ftc_detail_send_page_on_toggled_rdbtn_send_to_me(GtkToggleButton *toggle_button, gpointer data)
{
    FtcDetailSendPage *page = NULL;
    FtcDetailSendPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(data);

    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! toggle_button) {
        return;
    }

    if (gtk_toggle_button_get_active(toggle_button) == true) {
        gtk_label_set_text(priv->lbl_title_receiver_detail, "나에게");
        gtk_widget_set_visible(GTK_WIDGET(priv->imgbtn_select_target_user), false);
    }
}

static void 
ftc_detail_send_page_on_toggled_rdbtn_send_to_other(GtkToggleButton *toggle_button, gpointer data)
{
    FtcDetailSendPage *page = NULL;
    FtcDetailSendPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(data)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(data);

    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }
    if (! toggle_button) {
        return;
    }

    if (gtk_toggle_button_get_active(toggle_button) == true) {
        ftc_detail_send_page_set_receiver_detail(page);
        gtk_widget_set_visible(GTK_WIDGET(priv->imgbtn_select_target_user), true);
    }
}

UploadOption* 
ftc_detail_send_page_get_upload_option(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    GtkTextIter start, end;
    GtkTextBuffer *buffer = NULL;
    char *text = NULL;
    UploadOption* rv = NULL;
    JsonObject *jobj = NULL;
    std::vector<Ftc::Core::GlobalStruct::FtcNetwork> vc_network_list;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    buffer = gtk_text_view_get_buffer(priv->txtview_send_reason);
    if (! buffer) {
        return rv;
    }

    gtk_text_buffer_get_bounds(buffer, &start, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, false);

    priv->upload_option->setSendReason(text);

    //  내게 보내기 일 경우, 목록을 삭제하고 자신의 계정으로 설정한다.
    //
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->rdbtn_send_to_me))) {
        Ftc::Core::GlobalStruct::FtcUser ftc_user;
        const Ftc::Core::User &user = Ftc::Core::GlobalVar::getUser();

        priv->upload_option->clearUser();

        ftc_user.user_id = user.getUserId();
        ftc_user.user_name = user.getContact();
        ftc_user.is_group = false;
        ftc_user.is_include_sub_group = false;
        priv->upload_option->addUser(ftc_user);
    }
    
    vc_network_list = ftc_detail_send_page_add_upload_option_checked_network(page);
    priv->upload_option->setNetworks(vc_network_list);

    jobj = priv->pack_file_pop->getApprovalData();
    if (! jobj) {
        return rv;
    }

    priv->upload_option->setApprovalLines(jobj);

    if (text) {
        g_free(text);
    }

    rv = priv->upload_option;
    return rv;
}

bool                    
ftc_detail_send_page_is_approval_update(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    bool rv = false;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    rv = priv->is_approval_update;    

    return rv;
}

static void 
ftc_detail_send_page_set_network_check(FtcDetailSendPage *page, const char *network_name)
{
    FtcDetailSendPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! network_name) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->box_target_network_list), ftc_detail_send_page_set_network_check_callback, (gpointer)network_name);
}

static void 
ftc_detail_send_page_set_network_check_callback(GtkWidget *widget, gpointer data)
{
    GtkToggleButton *btn = NULL;
    const char *network_name = NULL;

    if (! widget || ! data) {
        return;
    }

    btn = GTK_TOGGLE_BUTTON(widget);
    network_name = (const char*)data;

    if (strcmp(gtk_button_get_label(GTK_BUTTON(btn)), network_name) == 0) {
        gtk_toggle_button_set_active(btn, true);
    }
}

static void 
ftc_detail_send_page_set_network_uncheck_all(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->box_target_network_list), ftc_detail_send_page_set_network_uncheck_all_callback, NULL);
}

static void 
ftc_detail_send_page_set_network_uncheck_all_callback(GtkWidget *widget, gpointer data)
{
    GtkToggleButton *btn = NULL;

    if (! widget) {
        return;
    }

    btn = GTK_TOGGLE_BUTTON(widget);
    gtk_toggle_button_set_active(btn, false);
}

static std::vector<Ftc::Core::GlobalStruct::FtcNetwork>            
ftc_detail_send_page_add_upload_option_checked_network(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    GetCheckNetworkParam param;
    std::vector<Ftc::Core::GlobalStruct::FtcNetwork> rv;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    param.page = page;

    gtk_container_foreach(GTK_CONTAINER(priv->box_target_network_list), ftc_detail_send_page_add_upload_option_checked_network_callback, &param);

    rv = param.vc_network_list;
    return rv; 
}

static void 
ftc_detail_send_page_add_upload_option_checked_network_callback(GtkWidget *widget, gpointer data)
{
    FtcDetailSendPage *page = NULL;
    GtkToggleButton *btn = NULL;
    GetCheckNetworkParam *param = NULL;

    if (! widget || ! data) {
        return;
    }

    param = (GetCheckNetworkParam*)data;
    
    if (! FTC_IS_DETAIL_SEND_PAGE(param->page)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(param->page);

    btn = GTK_TOGGLE_BUTTON(widget);
    
    if (gtk_toggle_button_get_active(btn) == true) {
        Ftc::Core::GlobalStruct::FtcNetwork insert_data = ftc_detail_send_page_find_ftc_network_from_network_name(page, gtk_button_get_label(GTK_BUTTON(btn)));
        param->vc_network_list.push_back(insert_data);
    }
}

static void 
ftc_detail_send_page_delete_all_list_network_data(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    g_slist_foreach(priv->network_list, ftc_detail_send_page_delete_all_list_network_data_callback, NULL);
}

static void 
ftc_detail_send_page_delete_all_list_network_data_callback(gpointer data, gpointer user_data)
{
    if (! data) {
        return;
    }

    delete (NetworkData*)data;
}

static const NetworkData*
ftc_detail_send_page_find_network_data_from_id(FtcDetailSendPage *page, const char *network_id)
{
    FtcDetailSendPagePrivate *priv = NULL;
    NetworkFindParam param;
    NetworkData* rv = NULL;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }
    if (! network_id) {
        return rv;
    }

    param.network_id = network_id;
    param.network_name = NULL;
    param.network_data = NULL;

    g_slist_foreach(priv->network_list, ftc_detail_send_page_find_network_data_from_id_callback, (gpointer)&param);

    return param.network_data;
}

static void
ftc_detail_send_page_find_network_data_from_id_callback(gpointer data, gpointer user_data)
{
    NetworkData *network_data = NULL;
    NetworkFindParam *param = NULL;

    if (! user_data || ! data) {
        return;
    }
    
    network_data = (NetworkData*)data;
    param = (NetworkFindParam*)user_data;

    if (network_data->to_network_id == param->network_id) {
        param->network_data = network_data;
    }
}

static Ftc::Core::GlobalStruct::FtcNetwork 
ftc_detail_send_page_find_ftc_network_from_network_name(FtcDetailSendPage *page, const char *network_name)
{
    FtcDetailSendPagePrivate *priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    std::list<Ftc::Core::GlobalStruct::FtcNetwork> lst_network; 
    Ftc::Core::GlobalStruct::FtcNetwork rv;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return rv;
    }
    if (! priv) {
        return rv;
    }

    lst_network = priv->pack_file_pop->getListFtcNetwork();

    for(auto &it : lst_network) {
        if (it.to_name == network_name) {
            rv = it;
            return rv;
        }
    }
}

static void 
ftc_detail_send_page_select_receive_user_webview_event(Ftc::Core::Event *evt)
{
    FtcDetailSendPage *page = NULL;
    FtcDetailSendPagePrivate *priv = NULL;
    WebViewResultParam *param = NULL;

    if (!evt) {
        return;
    }

    param = (WebViewResultParam*)evt->getUserData();
    if (!param) {
        return;
    }
    if (!param->node || !param->user_data) {
        return;
    }

    if (! FTC_IS_DETAIL_SEND_PAGE(param->user_data)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(param->user_data);

    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! param->node) {
        return;
    }
    ftc_detail_send_page_parse_user_select_values(page, param->node);
    ftc_detail_send_page_set_receiver_detail(page);
}

static void 
ftc_detail_send_page_parse_user_select_values(FtcDetailSendPage *page, JsonNode *node)
{
    FtcDetailSendPagePrivate *priv = NULL;
    std::list<std::string> list_split;
    std::string value, tmp;
    JsonObject *obj_data = NULL;
    JsonArray *arr_values = NULL;
    int arr_size = 0, start = 0, end = 0;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    obj_data = Ftc::Core::JsonUtils::getObject(node, "data");
    if (!obj_data) {
        return;
    }
    arr_values = Ftc::Core::JsonUtils::getArray(obj_data, "selectValues");
    if (!arr_values) {
        return;
    }

    priv->upload_option->clearUser();
    
    arr_size = json_array_get_length(arr_values);
    for (int i = 0; i < arr_size; i++) {
        int count = 0;
        Ftc::Core::GlobalStruct::FtcUser user;
        value = json_array_get_string_element(arr_values, i);
        list_split = Ftc::Core::StringUtils::split(value, '|');

        for (auto &it : list_split) {
            if (count == 0) {
                //  유저이름
                end = it.find("[");
                tmp = it.substr(0, end);
                Ftc::Core::StringUtils::trim(tmp);
                user.user_name = tmp;

                //  유저아이디
                start = end;
                end = it.find("]");
                tmp = it.substr(start + 1, end - start - 1);
                Ftc::Core::StringUtils::trim(tmp);
                user.user_id = tmp;
            } else if (count == 1) {
                user.is_group = (it == "Y") ? true : false;
            } else if (count == 2) {
                user.is_include_sub_group = (it == "Y") ? true : false;
            }
            count++;
        }
        priv->upload_option->addUser(user);
    }
}

static void 
ftc_detail_send_page_set_receiver_detail(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    std::string receiver;
    std::stringstream ss_output, ss_tooltip;
    int user_size = 0;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    user_size = priv->upload_option->getUserSize();

    for (int i = 0; i < priv->upload_option->getUserSize(); i++) {
        Ftc::Core::GlobalStruct::FtcUser user = priv->upload_option->getUser(i);

        if (i != 0) {
            ss_tooltip << "\n";
        }
        
        ss_tooltip << user.user_name;
        if (user.is_group == false) {
            ss_tooltip << "[";
            ss_tooltip << user.user_id;
            ss_tooltip << "]";
        }

        if (i == 0) {
            receiver = ss_tooltip.str();
        }
    }

    ss_output << receiver;
    if (user_size > 1) {    
        //  외 %d명
        ss_output << " ";
        ss_output << Ftc::Core::StringUtils::format("외 %d명", user_size - 1);
    }
    
    gtk_label_set_text(priv->lbl_title_receiver_detail, ss_output.str().c_str());
    gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_title_receiver_detail), ss_tooltip.str().c_str());
}

static void 
ftc_detail_send_page_approval_line_config_webview_event(Ftc::Core::Event *evt)
{
    FtcDetailSendPage *page = NULL;
    FtcDetailSendPagePrivate *priv = NULL;
    WebViewResultParam *param = NULL;
    JsonObject *jobj_data = NULL;
    
    if (! evt) {
        return;
    }

    param = (WebViewResultParam*)evt->getUserData();
    if (! param) {
        return;
    }
    if (! param->node || ! param->user_data) {
        return;
    }

    if (! FTC_IS_DETAIL_SEND_PAGE(param->user_data)) {
        return;
    }
    page = FTC_DETAIL_SEND_PAGE(param->user_data);

    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    jobj_data = json_node_get_object(param->node);
    if (! jobj_data) {
        return;
    }

    priv->pack_file_pop->setApproval(jobj_data);
    priv->is_approval_update = true;
}

static std::string 
ftc_detail_send_page_get_check_network_list_string(FtcDetailSendPage *page)
{
    FtcDetailSendPagePrivate *priv = NULL;
    std::string rv;

    if (! FTC_IS_DETAIL_SEND_PAGE(page)) {
        return rv;
    }
    priv = (FtcDetailSendPagePrivate*)ftc_detail_send_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->box_target_network_list), ftc_detail_send_page_get_check_network_list_string_callback, &rv);
}

static void 
ftc_detail_send_page_get_check_network_list_string_callback(GtkWidget *widget, gpointer data)
{
    std::string *str = NULL;
    const char *network_name = NULL;

    if (! data) {
        return;
    }
    str = (std::string*)data;

    if (! GTK_IS_TOGGLE_BUTTON(widget)) {
        return;
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == true) {
        network_name = gtk_button_get_label(GTK_BUTTON(widget));
        if (str->length() > 0) {
            str->append(", ");
        }
        str->append(network_name);
    }
}