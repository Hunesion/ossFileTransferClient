#include "ftchomepage.h"
#include "ftchomeheadersendbox.h"
#include "ftchomedndsubpage.h"
#include "ftchomeheaderrecvbox.h"
#include "ftchomeheadersendhistorybox.h"
#include "ftcmainwindow.h"
#include "../core/TimeUtils.h"


struct _FtcHomePage
{
    GtkBox parent;
};

typedef struct _FtcHomePagePrivate FtcHomePagePrivate;

struct _FtcHomePagePrivate
{
    /* Template에서 정의 */
    GtkBox      *box_header_user;
    GtkEventBox *event_box_user_menu;
    GtkEventBox *event_box_toolbar_menu;

    GtkImage    *img_user;
    GtkLabel    *lbl_user_menu;
    GtkImage    *imgbtn_user_menu;
    GtkImage    *imgbtn_toolbar_menu;
    GtkStack    *stack_list_send_recv;
    GtkStack    *stack_header_send_recv;

    /* 소스코드에서 정의 */
    FtcHomeHeaderSendBox *header_send_box;
    FtcHomeHeaderRecvBox *header_recv_box;
    FtcHomeHeaderSendHistoryBox *header_send_history_box;

    FtcHomeDndSubPage *dnd_subpage;

    //  유저 메뉴
    GtkMenu     *menu_user;
    //  [파일전송정책]
    GtkMenuItem *menu_it_my_file_send_policy;
    //  [비밀번호변경]
    GtkMenuItem *menu_it_change_password;
    //  [로그아웃]
    GtkMenuItem *menu_it_user_logout;

    //  툴바 메뉴
    GtkMenu     *menu_toolbar;
    //  [보낸이력]]
    GtkMenuItem *menu_it_send_history;
    //  [클립보드보내기]
    GtkMenuItem *menu_it_send_clipboard;
    //  [URL 보내기]
    GtkMenuItem *menu_it_send_url;
    //  [허용URL 확인]
    GtkMenuItem *menu_it_allow_url;
    //  [이력조회]
    GtkMenuItem *menu_it_lookup_history;
    //  이력조회 하위 메뉴
    GtkMenu     *menu_sub_lookup_history;
    //  [이력조회] - [사용자이력]
    GtkMenuItem *menu_sub_it_user_history;
    //  [이력조회] - [파일송수신이력]
    GtkMenuItem *menu_sub_it_file_transper_history;
    //  [이력조회] - [개인정보탐지이력]
    GtkMenuItem *menu_sub_it_detect_privacy_history;
    //  [요청관리]
    GtkMenuItem *menu_it_request_management;
    //  요청관리 하위 메뉴
    GtkMenu     *menu_sub_request_management;
    //  [요청관리] - [용량변경신청]
    GtkMenuItem *menu_sub_it_request_change_volume;
    //  [요청관리] - [요청목록]
    GtkMenuItem *menu_sub_it_request_list;
    //  [공지사항]
    GtkMenuItem *menu_it_notice;
    //  [버전정보]
    GtkMenuItem *menu_it_version_info;
    //  [종료]
    GtkMenuItem *menu_it_exit_application;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomePage, ftc_home_page, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_page_set_menu_user(FtcHomePage *page);

static void ftc_home_page_set_menu_toolbar(FtcHomePage *page);

static void ftc_home_page_update_sub_page_event(Ftc::Core::Event *evt);

static void ftc_home_page_send_list_add_file_event(Ftc::Core::Event *evt);

static void ftc_home_page_send_history_refresh_event(Ftc::Core::Event *evt);

static void ftc_home_page_show_last_recv_request_event(Ftc::Core::Event *evt);

static void ftc_home_page_recv_request_new_count(Ftc::Core::Event *evt);

static void ftc_home_page_send_history_new_count(Ftc::Core::Event *evt);

static void ftc_home_page_recv_download_all_event(Ftc::Core::Event *evt);

static void ftc_home_page_on_activate_menu_it_my_file_send_policy(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_change_password(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_user_logout(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_send_history(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_send_clipboard(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_send_url(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_allow_url(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_sub_it_user_history(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_sub_it_file_transper_history(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_sub_it_detect_privacy_history(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_sub_it_request_change_volume(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_sub_it_request_list(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_notice(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_version_info(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_activate_menu_it_exit_application(GtkWidget *widget, gpointer data);

static void ftc_home_page_on_button_press_event_box_user_menu(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_home_page_on_button_press_event_box_toolbar_menu(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_home_page_set_recv_request_new_count(FtcHomePage *page);

static void ftc_home_page_show_send_history(FtcHomePage *page);
///////////////////////////////////////////////////////////////////

static void
ftc_home_page_init (FtcHomePage *page)
{
    FtcHomePagePrivate *priv = NULL;
    std::string user_name;
    
    Ftc::Core::EventManager *event_mgr = Ftc::Core::EventManager::getInstance();

    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private (page);

    //  Todo..
    //  유저 메뉴 설정
    ftc_home_page_set_menu_user(page);

    //  Signal (Event) 함수 연결
    //  lbl_user_menu(event_box_user_menu) click 
    g_signal_connect(priv->event_box_user_menu, "button-press-event", G_CALLBACK(ftc_home_page_on_button_press_event_box_user_menu), page);

    //  imgbtn_toolbar_menu(event_box_toolbar_menu) click
    g_signal_connect(priv->event_box_toolbar_menu, "button-press-event", G_CALLBACK(ftc_home_page_on_button_press_event_box_toolbar_menu), page);

    //  이벤트 리스너 추가
    if (event_mgr) {
        event_mgr->addEventListener(page, FTC_UI_UPDATE_HOME_STACK_EVENT, HUNE_CALLBACK_CLS_STATIC_1(ftc_home_page_update_sub_page_event));
        event_mgr->addEventListener(page, FTC_UI_SEND_LIST_ADD_FILE, HUNE_CALLBACK_CLS_STATIC_1(ftc_home_page_send_list_add_file_event));
        event_mgr->addEventListener(page, FTC_UI_SEND_HISTORY_REFRESH, HUNE_CALLBACK_CLS_STATIC_1(ftc_home_page_send_history_refresh_event));
        event_mgr->addEventListener(page, FTC_UI_SHOW_LAST_RECV_REQUEST, HUNE_CALLBACK_CLS_STATIC_1(ftc_home_page_show_last_recv_request_event));
        event_mgr->addEventListener(page, FTC_UI_RECV_DOWNLOAD_ALL, HUNE_CALLBACK_CLS_STATIC_1(ftc_home_page_recv_download_all_event));
        event_mgr->addEventListener(page, FTC_UI_RECV_REQUEST_NEW_COUNT, HUNE_CALLBACK_CLS_STATIC_1(ftc_home_page_recv_request_new_count));
        event_mgr->addEventListener(page, FTC_UI_SEND_HISTORY_NEW_COUNT, HUNE_CALLBACK_CLS_STATIC_1(ftc_home_page_send_history_new_count));
    }

    //  디자인 설정
    //  위젯 크기 설정
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_header_user), 420, 45);
    gtk_widget_set_size_request(GTK_WIDGET(priv->stack_header_send_recv), 420, 45);

    //  CSS 설정
    if (ftc_ui_is_internal_location()) {
        //  업무 망

        //  유저 이름 부분 헤더 색상 설정
        gtk_widget_set_name(GTK_WIDGET(priv->box_header_user), "box_user_header_is");
        //  이미지 및 버튼
        gtk_image_set_from_resource(priv->img_user, "/com/hunesion/ftcapp/UESER-ICON-IS.png");
    } else {
        //  인터넷 망

        //  유저 이름 부분 헤더 색상 설정
        gtk_widget_set_name(GTK_WIDGET(priv->box_header_user), "box_user_header_es");
        //  이미지 및 버튼
        gtk_image_set_from_resource(priv->img_user, "/com/hunesion/ftcapp/UESER-ICON-ES.png");
    }
    //  유저 메뉴(이름) 라벨
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_user_menu), "label_user_menu");

    gtk_widget_set_name(GTK_WIDGET(priv->stack_header_send_recv), "default_background_page");

    //  버튼 이미지 설정
    gtk_image_set_from_resource(priv->imgbtn_user_menu, "/com/hunesion/ftcapp/ARROW-DOWN.png");
    gtk_image_set_from_resource(priv->imgbtn_toolbar_menu, "/com/hunesion/ftcapp/LIST-ICON-16PX.png");

    //  User Header에 유저이름과 망이름 설정
    user_name = Ftc::Core::GlobalVar::getUser().getContact() + "(" + Ftc::Core::GlobalVar::getUser().getUserId() + ")" "님";
    gtk_label_set_label(priv->lbl_user_menu, user_name.c_str());

    //  stack_send_info 에 기본 drag and drop & recv 화면 설정
    priv->dnd_subpage = ftc_home_dnd_sub_page_new(page);
    gtk_stack_add_named(priv->stack_list_send_recv, GTK_WIDGET(priv->dnd_subpage), FTC_VIEW_HOME_SUB_PAGE_DND);

    //  DND 페이지를 보여준다.
    gtk_stack_set_visible_child_name(priv->stack_list_send_recv, FTC_VIEW_HOME_SUB_PAGE_DND);
}

static void
ftc_home_page_dispose (GObject *object)
{
    FtcHomePagePrivate *priv;
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(FTC_HOME_PAGE(object));

    if (priv->menu_user != NULL) {
        //  안에서 하위 메뉴들 Ref 감소해줌
        gtk_widget_destroy(GTK_WIDGET(priv->menu_user));
        priv->menu_user = NULL;
    }

    if (priv->menu_toolbar != NULL) {
        //  안에서 하위 메뉴들 Ref 감소해줌
        gtk_widget_destroy(GTK_WIDGET(priv->menu_toolbar));
        priv->menu_toolbar = NULL;
    }

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);

    G_OBJECT_CLASS(ftc_home_page_parent_class)->dispose(object);
}

static void
ftc_home_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_home_page_parent_class)->finalize(object);
}

static void
ftc_home_page_class_init (FtcHomePageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-page.ui");
    
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, img_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, lbl_user_menu);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, imgbtn_user_menu);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, imgbtn_toolbar_menu);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, stack_list_send_recv);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, stack_header_send_recv);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, box_header_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, event_box_user_menu);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomePage, event_box_toolbar_menu);
}

FtcHomePage *
ftc_home_page_new ()
{
    FtcHomePage *page = FTC_HOME_PAGE(g_object_new (FTC_HOME_PAGE_TYPE, NULL));
    FtcHomePagePrivate *priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private (page);

    return page;
}

static void 
ftc_home_page_set_menu_user(FtcHomePage *page)
{
    FtcHomePagePrivate *priv = NULL; 

    if (! FTC_IS_HOME_PAGE(page)) {
        return;
    }

    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    //  유저메뉴
    priv->menu_user = GTK_MENU(gtk_menu_new());

    //  내 파일전송 정책
    priv->menu_it_my_file_send_policy = GTK_MENU_ITEM(gtk_menu_item_new_with_label("내 파일전송 정책"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_user), GTK_WIDGET(priv->menu_it_my_file_send_policy));
    //  menu_it_my_file_send_policy menu click [내 파일전송 정책]
    g_signal_connect(priv->menu_it_my_file_send_policy, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_my_file_send_policy), page);

    //  비밀번호 변경
    priv->menu_it_change_password = GTK_MENU_ITEM(gtk_menu_item_new_with_label("비밀번호 변경"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_user), GTK_WIDGET(priv->menu_it_change_password));
    //  menu_it_change_password menu click [비밀번호 변경]
    g_signal_connect(priv->menu_it_change_password, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_change_password), page);

    //  로그아웃
    priv->menu_it_user_logout = GTK_MENU_ITEM(gtk_menu_item_new_with_label("로그아웃"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_user), GTK_WIDGET(priv->menu_it_user_logout));
    //  menu_it_user_logout menu click [로그아웃]
    g_signal_connect(priv->menu_it_user_logout, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_user_logout), page);


    gtk_widget_show_all(GTK_WIDGET(priv->menu_user));
}

static void 
ftc_home_page_set_menu_toolbar(FtcHomePage *page)
{
    FtcHomePagePrivate *priv = NULL;
    std::tm to_date_tm = Ftc::Core::TimeUtils::now();
    std::tm from_date_tm = Ftc::Core::TimeUtils::addDay(to_date_tm, -30);
    bool left_menu_success = false;
    std::string str;
    int approval_stand_by_cnt = -1, approval_after_check_cnt = -1, approval_user_stand_by_cnt = -1, approval_user_after_check_cnt = -1;
    

    if (! FTC_IS_HOME_PAGE(page)) {
        return;
    }
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    if (priv->menu_toolbar) {
        //  안에서 하위 메뉴들 Ref 감소해줌
        gtk_widget_destroy(GTK_WIDGET(priv->menu_toolbar));
    }
    priv->menu_toolbar = GTK_MENU(gtk_menu_new());
    
    //  보낸이력
    priv->menu_it_send_history = GTK_MENU_ITEM(gtk_menu_item_new_with_label("보낸이력"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_toolbar), GTK_WIDGET(priv->menu_it_send_history));
    //  menu_it_send_history menu click [보낸이력]
    g_signal_connect(priv->menu_it_send_history, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_send_history), page);

    //  클립보드 보내기
    if (ftc_ui_is_use_clipboard()) {
        priv->menu_it_send_clipboard = GTK_MENU_ITEM(gtk_menu_item_new_with_label("클립보드 보내기"));
        gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_toolbar), GTK_WIDGET(priv->menu_it_send_clipboard));
        //  menu_it_send_clipboard menu click [클립보드 보내기]
        g_signal_connect(priv->menu_it_send_clipboard, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_send_clipboard), page);
    }
    
    if (ftc_ui_is_use_url_redirection()) {
        //  URL 보내기
        priv->menu_it_send_url = GTK_MENU_ITEM(gtk_menu_item_new_with_label("URL 보내기"));
        gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_toolbar), GTK_WIDGET(priv->menu_it_send_url));
        //  menu_it_send_url menu click [URL 보내기]
        g_signal_connect(priv->menu_it_send_url, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_send_url), page);
    }

    //  이력조회
    priv->menu_it_lookup_history = GTK_MENU_ITEM(gtk_menu_item_new_with_label("이력조회"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_toolbar), GTK_WIDGET(priv->menu_it_lookup_history));
    //  이력조회 서브메뉴
    priv->menu_sub_lookup_history = GTK_MENU(gtk_menu_new());
    gtk_menu_item_set_submenu(priv->menu_it_lookup_history, GTK_WIDGET(priv->menu_sub_lookup_history));
    //  이력조회 - 사용자이력
    priv->menu_sub_it_user_history = GTK_MENU_ITEM(gtk_menu_item_new_with_label("사용자이력"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_sub_lookup_history), GTK_WIDGET(priv->menu_sub_it_user_history));
    //  menu_sub_it_user_history menu click [이력조회 - 사용자이력]
    g_signal_connect(priv->menu_sub_it_user_history, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_sub_it_user_history), page);
    //  이력조회 - 파일송수신이력
    priv->menu_sub_it_file_transper_history = GTK_MENU_ITEM(gtk_menu_item_new_with_label("파일송수신이력"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_sub_lookup_history), GTK_WIDGET(priv->menu_sub_it_file_transper_history));
    //  menu_sub_it_file_transper_history menu click [이력조회 - 파일송수신이력]
    g_signal_connect(priv->menu_sub_it_file_transper_history, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_sub_it_file_transper_history), page);
    
    //  공지사항
    priv->menu_it_notice = GTK_MENU_ITEM(gtk_menu_item_new_with_label("공지사항"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_toolbar), GTK_WIDGET(priv->menu_it_notice));
    //  menu_it_notice menu click [공지사항]
    g_signal_connect(priv->menu_it_notice, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_notice), page);
    
    //  버전정보
    priv->menu_it_version_info = GTK_MENU_ITEM(gtk_menu_item_new_with_label("버전정보"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_toolbar), GTK_WIDGET(priv->menu_it_version_info));
    //  menu_it_version_info menu click [버전정보]
    g_signal_connect(priv->menu_it_version_info, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_version_info), page);
    
    //  종료
    priv->menu_it_exit_application = GTK_MENU_ITEM(gtk_menu_item_new_with_label("종료"));
    gtk_menu_shell_append(GTK_MENU_SHELL(priv->menu_toolbar), GTK_WIDGET(priv->menu_it_exit_application));
    //  menu_it_exit_application menu click [종료]
    g_signal_connect(priv->menu_it_exit_application, "activate", G_CALLBACK(ftc_home_page_on_activate_menu_it_exit_application), page);

    gtk_widget_show_all(GTK_WIDGET(priv->menu_toolbar));
}

const char*
ftc_home_page_get_current_page(FtcHomePage *page)
{
    FtcHomePagePrivate *priv = NULL;
    const char *rv = NULL;
    
    if (! FTC_IS_HOME_PAGE(page)) {
        return rv;
    }
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private (page);
    if (! priv) {
        return rv;
    }

    rv = gtk_stack_get_visible_child_name(priv->stack_list_send_recv);
    return rv;
}

static void 
ftc_home_page_update_sub_page_event(Ftc::Core::Event *evt)
{
    UpdatePageEventParam *param = NULL;
    FtcHomePage *page = NULL;
    FtcHomePagePrivate *priv = NULL;
    const char *child_name = NULL;

    if (! evt) {
        return;
    }
    if (! evt->getUserData()) {
        return;
    }

    param = (UpdatePageEventParam*)evt->getUserData();
    if (! FTC_IS_HOME_PAGE(param->ftc_object)) {
        return;
    }
    page = FTC_HOME_PAGE(param->ftc_object);

    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    child_name = gtk_stack_get_visible_child_name(GTK_STACK(priv->stack_header_send_recv));

    if (param->page_name == FTC_VIEW_HOME_SUB_PAGE_DND) {
        if (GTK_IS_STACK(priv->stack_list_send_recv) && FTC_IS_HOME_DND_SUB_PAGE(priv->dnd_subpage)) {
            if ((strcmp(child_name, FTC_VIEW_HOME_HEADER_SEND) != 0) && GTK_IS_STACK(priv->stack_header_send_recv)) {
                gtk_stack_set_visible_child_name(GTK_STACK(priv->stack_header_send_recv), FTC_VIEW_HOME_HEADER_SEND);
            }
            if (ftc_ui_is_extension_widnow() == true) {
                ftc_ui_set_extension_window(ftc_ui_get_main_window(), false);
            }
            gtk_stack_set_visible_child_name(GTK_STACK(priv->stack_list_send_recv), FTC_VIEW_HOME_SUB_PAGE_DND);
        }
    } else if (param->page_name == FTC_VIEW_HOME_SUB_PAGE_SEND_LIST) {
        
    } else if (param->page_name == FTC_VIEW_HOME_SUB_PAGE_RECV_LIST) {    
        
    } else if (param->page_name == FTC_VIEW_HOME_SUB_PAGE_SEND_HISTORY) {
        
    }
}

static void
ftc_home_page_send_list_add_file_event(Ftc::Core::Event *evt)
{
    SendListEventParam *param = NULL;
    FtcHomePage *page = NULL;
    FtcHomePagePrivate *priv = NULL;

    if (! evt) {
        return;
    }
    if (! evt->getUserData()) {
        return;
    }
    param = (SendListEventParam*)evt->getUserData();

    if (! FTC_IS_HOME_PAGE(param->home_page)) {
        return;
    }
    page = FTC_HOME_PAGE(param->home_page);

    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

}

void           
ftc_home_page_recv_list_refresh(FtcHomePage *page)
{
    FtcHomePagePrivate *priv = NULL;

    if (! FTC_IS_HOME_PAGE(page)) {
        return;
    }
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

}

static void 
ftc_home_page_send_history_refresh_event(Ftc::Core::Event *evt)
{
    FtcHomePage *page = NULL;
    FtcHomePagePrivate *priv = NULL;

    if (! evt) {
        return;
    }
    if (! FTC_IS_HOME_PAGE(evt->getUserData())) {
        return;
    }

    page = FTC_HOME_PAGE(evt->getUserData());

    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

}

static void
ftc_home_page_show_last_recv_request_event(Ftc::Core::Event *evt) {
    RecvRequestEventParam *param = NULL;
    FtcHomePage *page = NULL;
    FtcHomePagePrivate *priv = NULL;

    if (! evt) {
        return;
    }
    if (! evt->getUserData()) {
        return;
    }
    param = (RecvRequestEventParam*)evt->getUserData();

    if (! FTC_IS_HOME_PAGE(param->ftc_object)) {
        return;
    }
    page = FTC_HOME_PAGE(param->ftc_object);
    
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (priv->dnd_subpage) {
        if (! param->ftc_request) {
            return;
        }
        ftc_home_dnd_sub_page_update_last_request(priv->dnd_subpage, (Ftc::Core::GlobalStruct::FtcRequest*)param->ftc_request);
    }

    ftc_home_page_set_recv_request_new_count(page);
}

static void 
ftc_home_page_recv_request_new_count(Ftc::Core::Event *evt)
{
    RecvRequestEventParam *param = NULL;
    FtcHomePage *page = NULL;

    if (!evt) {
        return;
    }
    if (! evt->getUserData()) {
        return;
    }

    param = (RecvRequestEventParam*)evt->getUserData();
    
    if (! FTC_IS_HOME_PAGE(param->ftc_object)) {
        return;
    }
    page = FTC_HOME_PAGE(param->ftc_object);

    ftc_home_page_set_recv_request_new_count(page);
}

static void 
ftc_home_page_send_history_new_count(Ftc::Core::Event *evt)
{
    SendHistoryCountEventParam *param = NULL;
    FtcHomePage *page = NULL;
    FtcHomePagePrivate *priv = NULL;

    if (!evt) {
        return;
    }
    if (! evt->getUserData()) {
        return;
    }

    param = (SendHistoryCountEventParam*)evt->getUserData();
    
    if (! FTC_IS_HOME_PAGE(param->ftc_object)) {
        return;
    }
    page = FTC_HOME_PAGE(param->ftc_object);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    ftc_home_header_send_history_set_new_count(priv->header_send_history_box, param->count);
}

static void 
ftc_home_page_recv_download_all_event(Ftc::Core::Event *evt)
{
    RecvRequestEventParam *param = NULL;
    FtcHomePage *page = NULL;
    FtcHomePagePrivate *priv = NULL;
    const char *child_name = NULL;

    if (! evt) {
        return;
    }
    if (! evt->getUserData()) {
        return;
    }
    param = (RecvRequestEventParam*)evt->getUserData();
    
    if (! FTC_IS_HOME_PAGE(param->ftc_object)) {
        return;
    }
    page = FTC_HOME_PAGE(param->ftc_object);

    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! param->ftc_request) {
        return;
    }

    child_name = gtk_stack_get_visible_child_name(GTK_STACK(priv->stack_header_send_recv));
    if (strcmp(child_name, FTC_VIEW_HOME_HEADER_RECV) != 0) {
        gtk_stack_set_visible_child_name(GTK_STACK(priv->stack_header_send_recv), FTC_VIEW_HOME_HEADER_RECV);
    }
}

static void 
ftc_home_page_on_activate_menu_it_my_file_send_policy(GtkWidget *widget, gpointer data)
{
    GtkWindow *main_window = GTK_WINDOW(ftc_ui_get_main_window());
    std::string url;

    if (! main_window || ! widget || ! data) {
        return;
    }

    url = "INIT_PACK_FILE_POLICY_POP";
    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window),  url.c_str(), NULL, "내 파일전송 정책", 600, 370); 
}

static void 
ftc_home_page_on_activate_menu_it_change_password(GtkWidget *widget, gpointer data)
{
    GtkWindow *main_window = GTK_WINDOW(ftc_ui_get_main_window());
    std::string url;

    if (! main_window || ! widget || ! data) {
        return;
    }

    url = "UPDATE_PASSWORD_POP";

    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window),  url.c_str(), NULL, "비밀번호 변경", 420, 200); 
}

static void 
ftc_home_page_on_activate_menu_it_user_logout(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

}

static void 
ftc_home_page_on_activate_menu_it_send_history(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;
    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }


    
    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    ftc_home_page_show_send_history(page);
}

static void 
ftc_home_page_on_activate_menu_it_send_clipboard(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }
}

static void 
ftc_home_page_on_activate_menu_it_send_url(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (! event_mgr) {
        return;
    }

    event_mgr->dispatchEvent(FTC_UI_SEND_URL_REDIRECTION);
}

static void 
ftc_home_page_on_activate_menu_it_allow_url(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;
    HTTP_PARAM param;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    http_add_param(param, "is_primary", "");

    
    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "INIT_PC_URL_INFO_POP", &param, "허용URL 확인", 400, 400);
}

static void 
ftc_home_page_on_activate_menu_sub_it_user_history(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;
    HTTP_PARAM param;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    http_add_param(param, "main_page", "SELECT_LOGIN_INFO_LIST");
    http_add_param(param, "callName", "temp");

    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "COMMON_MAIN", &param, "사용자 이력", 1400, 400); 
}

static void 
ftc_home_page_on_activate_menu_sub_it_file_transper_history(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;
    HTTP_PARAM param;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    http_add_param(param, "main_page", "SELECT_USER_REQUEST_LIST");
    http_add_param(param, "callName", "temp");    

    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "COMMON_MAIN", &param, "파일송수신이력", 1400, 400); 
}

static void 
ftc_home_page_on_activate_menu_sub_it_detect_privacy_history(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

}

static void 
ftc_home_page_on_activate_menu_sub_it_request_change_volume(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "INIT_REQUEST_SIZE_POP", NULL, "용량변경신청", 400, 800); 
}

static void 
ftc_home_page_on_activate_menu_sub_it_request_list(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;
    HTTP_PARAM param;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    http_add_param(param, "main_page", "SELECT_USER_REQUEST_LIST");
    http_add_param(param, "callName", "ft_requestList");
    
    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "COMMON_MAIN", &param, "요청목록", 400, 800);
}

static void 
ftc_home_page_on_activate_menu_it_notice(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;
    HTTP_PARAM param;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    http_add_param(param, "main_page", "SELECT_BOARD_LIST");
    http_add_param(param, "callName", "temp");
    
    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "COMMON_MAIN", &param, "공지사항", 1400, 400);
}

static void 
ftc_home_page_on_activate_menu_it_version_info(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    gpointer main_window = NULL;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    main_window = ftc_ui_get_main_window();
    if (! main_window) {
        return;
    }

    ftc_web_kit_show_dialog_post(GTK_WINDOW(main_window), "VERSION_INFO_POP", NULL, "버전정보", 400, 200); 
}

static void 
ftc_home_page_on_activate_menu_it_exit_application(GtkWidget *widget, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (! event_mgr) {
        return;
    }

    event_mgr->dispatchEvent(FTC_UI_APPLICATION_QUIT, ftc_ui_get_application());
}

static void
ftc_home_page_on_button_press_event_box_user_menu(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;

    if (! FTC_IS_HOME_PAGE(data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);

    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! event || ! widget) {
        return;
    }
    event_button = (GdkEventButton*)event;
    
    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
       

        gtk_menu_popup_at_widget(priv->menu_user, widget, GdkGravity::GDK_GRAVITY_SOUTH, GdkGravity::GDK_GRAVITY_NORTH, NULL);

    }
}

static void
ftc_home_page_on_button_press_event_box_toolbar_menu(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomePage* page = NULL;
    FtcHomePagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;

    if (! FTC_IS_HOME_PAGE (data)) {
        return;
    }
    page = FTC_HOME_PAGE(data);

    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    ftc_home_page_set_menu_toolbar(page);
    
    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        gtk_menu_popup_at_widget(priv->menu_toolbar, GTK_WIDGET(priv->imgbtn_toolbar_menu), GdkGravity::GDK_GRAVITY_SOUTH, GdkGravity::GDK_GRAVITY_NORTH_WEST, NULL);
    }
}

static void 
ftc_home_page_set_recv_request_new_count(FtcHomePage *page)
{
    FtcHomePagePrivate *priv = NULL;
    int new_count = 0;

    if (! FTC_IS_HOME_PAGE(page)) {
        return;
    }
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (priv->dnd_subpage) {
        ftc_home_dnd_sub_page_set_new_recv_request_count(priv->dnd_subpage, new_count);
    }

    if (priv->header_recv_box) {
        ftc_home_header_recv_box_set_new_recv_request_count(priv->header_recv_box, new_count);
    }
}

static void
ftc_home_page_show_send_history(FtcHomePage *page)
{
    FtcHomePagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    UpdatePageEventParam update_page_evt_param;

    if (! FTC_IS_HOME_PAGE(page)) {
        return;
    }
    priv = (FtcHomePagePrivate*)ftc_home_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (! event_mgr) {
        return;
    }

    update_page_evt_param.ftc_object = page;
    update_page_evt_param.page_name = FTC_VIEW_HOME_SUB_PAGE_SEND_HISTORY;

    event_mgr->dispatchEvent(FTC_UI_UPDATE_HOME_STACK_EVENT, &update_page_evt_param);
}
