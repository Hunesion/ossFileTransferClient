#include "ftcloginpage.h"
#include "../core/sys_utils.h"
#include "../core/StringUtils.h"




struct _FtcLoginPage
{
    GtkBox parent;
};

typedef struct _FtcLoginPagePrivate FtcLoginPagePrivate;

struct _FtcLoginPagePrivate
{
    /* template에서 정의 */
    GtkLabel *lbl_location;
    GtkLabel *lbl_link_wall;
    GtkEntry *etr_id;
    GtkEntry *etr_password;
    GtkButton *btn_login;
    GtkCheckButton *ckbtn_save_id;
    GtkLinkButton *lbtn_reset_password;
    GtkLinkButton *lbtn_reg_user;
    GtkLinkButton *lbtn_get_manual;
    GtkBox        *box_lbtn_area;
    /* 소스에서 정의 */
    bool login_state;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcLoginPage, ftc_login_page, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static int  ftc_login_page_result_login_event(Ftc::Core::Event *evt);

static void ftc_login_page_on_click_btn_login(GtkWidget *widget, gpointer data);

static void ftc_login_page_on_change_etr_id(GtkWidget *widget, gpointer data);

static void ftc_login_page_on_change_etr_password(GtkWidget *widget, gpointer data);

static void ftc_login_page_on_key_release_event(GtkWidget *widget, GdkEventKey *data);

static void ftc_login_page_btn_login_activation(FtcLoginPage *page);

static void ftc_login_page_link_btn_placement(FtcLoginPage *page);

static void ftc_login_page_on_click_lbtn_reset_password (GtkWidget *widget, gpointer data);

static void ftc_login_page_on_click_lbtn_reg_user (GtkWidget *widget, gpointer data);

static void ftc_login_page_on_click_lbtn_get_manual (GtkWidget *widget, gpointer data);

static void ftc_login_page_set_config_auto_login_user_id(FtcLoginPage *page);

static void ftc_login_page_get_config_auto_login_user_id(FtcLoginPage *page);
///////////////////////////////////////////////////////////////////

static void
ftc_login_page_init (FtcLoginPage *page)
{
    FtcLoginPagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    std::string location_name;

    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private (page);
    event_mgr = Ftc::Core::EventManager::getInstance();
    priv->login_state = false;

    //  Todo..
    //  Signal (Event) 함수 연결
    //  gtkbox key release
    g_signal_connect(page, "key_release_event", G_CALLBACK(ftc_login_page_on_key_release_event), page);

    //  btn_login Click
    g_signal_connect(priv->btn_login, "clicked", G_CALLBACK(ftc_login_page_on_click_btn_login), page);

    //  etr_id change
    g_signal_connect(priv->etr_id, "changed", G_CALLBACK(ftc_login_page_on_change_etr_id), page);

    //  etr_password change
    g_signal_connect(priv->etr_password, "changed", G_CALLBACK(ftc_login_page_on_change_etr_password), page);

    //  lbtn_reset_password click
    g_signal_connect(priv->lbtn_reset_password, "clicked", G_CALLBACK(ftc_login_page_on_click_lbtn_reset_password), page);

    //  lbtn_reg_user click
    g_signal_connect(priv->lbtn_reg_user, "clicked", G_CALLBACK(ftc_login_page_on_click_lbtn_reg_user), page);

    //  lbtn_get_manual click
    g_signal_connect(priv->lbtn_get_manual, "clicked", G_CALLBACK(ftc_login_page_on_click_lbtn_get_manual), page);

    //  로그인 관련 HTTP 이벤트 등록

    //  Login 결과 이벤트 등록 ftc_login_result_login_event 함수 호출
    event_mgr->addEventListener(page, FTC_CORE_LOGIN_EVENT , HUNE_CALLBACK_CLS_STATIC_1(ftc_login_page_result_login_event));

    //  WIDGET 크기 셋팅
    gtk_widget_set_size_request(GTK_WIDGET(priv->etr_id), 320, 40);
    gtk_widget_set_size_request(GTK_WIDGET(priv->etr_password), 320, 40);
    gtk_widget_set_size_request(GTK_WIDGET(priv->btn_login), 320, 40);

    //  WIDGET 색상 디자인 셋팅
    //  네트워크 이름
    location_name = Ftc::Core::GlobalVar::getLoginLocation().getName();
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_location), "label_login_location");
    gtk_label_set_label(priv->lbl_location, location_name.c_str());

    //  Entry 텍스트
    gtk_widget_set_name(GTK_WIDGET(priv->etr_id), "entry_default");
    gtk_widget_set_name(GTK_WIDGET(priv->etr_password), "entry_default");

    //  로그인 버튼
    if (ftc_ui_is_internal_location()) {
        gtk_widget_set_name(GTK_WIDGET(priv->btn_login), "button_is");
    } else {
        gtk_widget_set_name(GTK_WIDGET(priv->btn_login), "button_es");
    }
    //  아이디 저장 체크박스
    gtk_widget_set_name(GTK_WIDGET(priv->ckbtn_save_id), "checkbox_id_save");
    //  링크버튼
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_link_wall), "link_button_login_page");
    gtk_widget_set_name(GTK_WIDGET(priv->lbtn_reset_password), "link_button_login_page");
    gtk_widget_set_name(GTK_WIDGET(priv->lbtn_reg_user), "link_button_login_page");
    gtk_widget_set_name(GTK_WIDGET(priv->lbtn_get_manual), "link_button_login_page");

    //  위치 설정
    ftc_login_page_link_btn_placement(page);

    //  버튼 비활성화
    ftc_login_page_btn_login_activation(page);

    //  아이디 저장된 값 가져오기
    ftc_login_page_get_config_auto_login_user_id(page);

}

static void
ftc_login_page_dispose (GObject *object)
{
    Ftc::Core::EventManager::getInstance()->removeEventListener(object);
    
    G_OBJECT_CLASS(ftc_login_page_parent_class)->dispose(object);
}

static void
ftc_login_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_login_page_parent_class)->finalize(object);
}

static void
ftc_login_page_class_init (FtcLoginPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_login_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_login_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-login-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, lbl_location);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, lbl_link_wall);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, etr_id);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, etr_password);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, btn_login);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, ckbtn_save_id);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, lbtn_reset_password);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, lbtn_reg_user);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, lbtn_get_manual);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcLoginPage, box_lbtn_area);
}

FtcLoginPage *
ftc_login_page_new ()
{
    FtcLoginPage *page = FTC_LOGIN_PAGE(g_object_new (FTC_LOGIN_PAGE_TYPE, NULL));
    FtcLoginPagePrivate *priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private (page);

    return page;
}

void
ftc_login_page_do_login(FtcLoginPage *page)
{
    FtcLoginPagePrivate *priv = NULL;
    std::string user_id, user_password;
    
    if (! FTC_IS_LOGIN_PAGE(page)) {
        return;
    }

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (priv->login_state == true) {
        return;
    }

    user_id = (char*)gtk_entry_get_text(priv->etr_id);
    if (strlen(user_id.c_str()) <= 0) {
        ftc_ui_message_box(GtkMessageType::GTK_MESSAGE_INFO, GtkButtonsType::GTK_BUTTONS_OK, "아이디를 입력해주십시오.");
        return;
    }
    Ftc::Core::StringUtils::trim(user_id);

    user_password = (char*)gtk_entry_get_text(priv->etr_password);
    if (strlen(user_password.c_str()) <= 0) {
        ftc_ui_message_box(GtkMessageType::GTK_MESSAGE_INFO, GtkButtonsType::GTK_BUTTONS_OK, "비밀번호를 입력해주십시오.");
        return;
    }
    Ftc::Core::StringUtils::trim(user_password);

    Ftc::Core::Request::requestLogin(user_id.c_str(), user_password.c_str(), false, page); 
}

static int
ftc_login_page_result_login_event(Ftc::Core::Event *evt)
{
    FtcLoginPage *page = NULL;
    FtcLoginPagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    UpdatePageEventParam update_page_evt_param;
    int rv = 0;

    if (! FTC_IS_LOGIN_PAGE(evt->getUserData())) {
        return rv;
    } 

    page = FTC_LOGIN_PAGE(evt->getUserData());

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    if (Ftc::Core::GlobalVar::getUser().isValid()) {
        FTC_LOG_DEBUG("Login Success "); 
        rv = 1;

        ftc_login_page_set_config_auto_login_user_id(page);

        event_mgr = Ftc::Core::EventManager::getInstance();
        if (! event_mgr) {
            return rv;
        }

        update_page_evt_param.ftc_object = ftc_ui_get_main_window();
        update_page_evt_param.page_name = FTC_VIEW_WINDOW_PAGE_HOME;
        event_mgr->dispatchEvent(FTC_UI_UPDATE_MAIN_STACK_EVENT, &update_page_evt_param);
    } else {
        FTC_LOG_DEBUG("Login Faild ");
        ftc_ui_message_box(GtkMessageType::GTK_MESSAGE_INFO, GtkButtonsType::GTK_BUTTONS_OK, "ID 또는 Password가 일치하지 않습니다.");
    }

    //  패스워드 Entry에 있는 값을 초기화 후 포커스
    //
    gtk_entry_set_text(priv->etr_password, "");
    gtk_widget_grab_focus(GTK_WIDGET(priv->etr_password));

    return rv;
}


static void 
ftc_login_page_on_click_btn_login(GtkWidget *widget, gpointer data)
{
    FtcLoginPage *page = NULL;
    FtcLoginPagePrivate *priv = NULL;

    if (! FTC_IS_LOGIN_PAGE(data)) {
        return;
    }
    page = FTC_LOGIN_PAGE(data);

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return;
    }
    
    ftc_login_page_do_login(page);
}

static void 
ftc_login_page_on_change_etr_id(GtkWidget *widget, gpointer data)
{
    if (! FTC_IS_LOGIN_PAGE(data)) {
        return;
    }
    ftc_login_page_btn_login_activation(FTC_LOGIN_PAGE(data));
}

static void 
ftc_login_page_on_change_etr_password(GtkWidget *widget, gpointer data)
{
    if (! FTC_IS_LOGIN_PAGE(data)) {
        return;
    }
    ftc_login_page_btn_login_activation(FTC_LOGIN_PAGE(data));
}

static void
ftc_login_page_btn_login_activation(FtcLoginPage *page)
{
     FtcLoginPagePrivate *priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);

}

static void 
ftc_login_page_link_btn_placement(FtcLoginPage *page)
{
    FtcLoginPagePrivate *priv = NULL;
    bool visible_reg_user = false, visible_reset_password = false;

    if (! FTC_IS_LOGIN_PAGE(page)) {
        return;
    }

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    visible_reg_user = gtk_widget_get_visible(GTK_WIDGET(priv->lbtn_reg_user));
    visible_reset_password = gtk_widget_get_visible(GTK_WIDGET(priv->lbtn_reset_password));

    if (visible_reg_user && visible_reset_password) {
        //  둘다 활성화
        gtk_widget_set_margin_start(GTK_WIDGET(priv->box_lbtn_area), 115);
        gtk_widget_set_visible(GTK_WIDGET(priv->lbl_link_wall), true);
    } else if (visible_reset_password) {
        //  패스워드 초기화만 활성화
        gtk_widget_set_margin_start(GTK_WIDGET(priv->box_lbtn_area), 163);
    } else if (visible_reg_user) {
        //  사용자등록요청만 활성화
        gtk_widget_set_margin_start(GTK_WIDGET(priv->box_lbtn_area), 162);
    }
}

static void 
ftc_login_page_on_key_release_event(GtkWidget *widget, GdkEventKey *data)
{
    FtcLoginPage *page = NULL;
    FtcLoginPagePrivate *priv = NULL;

    if (! FTC_IS_LOGIN_PAGE(widget)) {
        return;
    }
    page = FTC_LOGIN_PAGE(widget);

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (data->keyval == GDK_KEY_Return)
    {
        ftc_login_page_do_login(page);
    }
}

static void 
ftc_login_page_on_click_lbtn_reset_password (GtkWidget *widget, gpointer data)
{
    gpointer win = ftc_ui_get_main_window();
    if (! win || !GTK_IS_WINDOW(win))  {
        return ; 
    }

    ftc_web_kit_show_dialog_post(GTK_WINDOW(win),  "UPDATE_RESET_PASSWORD", NULL, "비밀번호 초기화", 400, 250); 
}

static void 
ftc_login_page_on_click_lbtn_reg_user (GtkWidget *widget, gpointer data)
{
    gpointer win = ftc_ui_get_main_window(); 

    if (!win || !GTK_IS_WINDOW(win)) {
        return ; 
    }

    ftc_web_kit_show_dialog_post(GTK_WINDOW(win), "INIT_REQUEST_USER_POP", NULL, "사용자 등록신청", 1200, 780, NULL, NULL, false, false, true); 
}

static void
ftc_login_page_on_click_lbtn_get_manual (GtkWidget *widget, gpointer data)
{
    
}

static void
ftc_login_page_set_config_auto_login_user_id(FtcLoginPage *page)
{
    FtcLoginPagePrivate *priv = NULL;
    const char *userid = NULL;
    Ftc::Core::ClientConfig &client_config = Ftc::Core::GlobalVar::getClientConfig();

    if (! FTC_IS_LOGIN_PAGE(page)) {
        return;
    }

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->ckbtn_save_id)) == true) {
        userid = gtk_entry_get_text(priv->etr_id);
    }

    if (userid) {
        client_config.setUserId(userid);
    } else {
        client_config.setUserId("");
    }
}

static void
ftc_login_page_get_config_auto_login_user_id(FtcLoginPage *page)
{
    FtcLoginPagePrivate *priv = NULL;
    std::string userid;
    Ftc::Core::ClientConfig &client_config = Ftc::Core::GlobalVar::getClientConfig();

    if (! FTC_IS_LOGIN_PAGE(page)) {
        return;
    }

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    userid = client_config.getUserId();
    if (userid.length() > 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->ckbtn_save_id), true);
        gtk_entry_set_text(priv->etr_id, userid.c_str());
        gtk_widget_grab_focus(GTK_WIDGET(priv->etr_password));
    }
}

void 
ftc_login_page_set_login_state(FtcLoginPage *page, bool state)
{
    FtcLoginPagePrivate *priv = NULL;

    if (! FTC_IS_LOGIN_PAGE(page)) {
        return;
    }

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    priv->login_state = state;
}

bool 
ftc_login_page_get_login_state(FtcLoginPage *page)
{
    FtcLoginPagePrivate *priv = NULL;
    bool rv = false;

    if (! FTC_IS_LOGIN_PAGE(page)) {
        return rv;
    }

    priv = (FtcLoginPagePrivate*)ftc_login_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    rv = priv->login_state;
    return rv;
}
