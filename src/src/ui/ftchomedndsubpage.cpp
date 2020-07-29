#include "ftchomedndsubpage.h"
#include "ftchomeheaderrecvbox.h"
#include "ftcmainwindow.h"

#include "../core/StringUtils.h"


struct _FtcHomeDndSubPage
{
    GtkBox parent;
};

typedef struct _FtcHomeDndSubPagePrivate FtcHomeDndSubPagePrivate;

struct _FtcHomeDndSubPagePrivate
{
    FtcHomePage *home_page;
    FtcHomeHeaderRecvBox *header_recv_box;

    GtkBox      *box_dnd_area;
    GtkBox      *box_recv_info;
    GtkBox      *box_dnd_border;

    GtkEventBox *event_box_dnd_area;
    GtkEventBox *event_box_message_mode;
    GtkEventBox *event_box_batch_download;

    GtkStack    *stack_header_recv;

    GtkImage    *img_file_add;
    GtkImage    *img_message_mode;
    GtkImage    *imgbtn_batch_download;
    GtkLabel    *lbl_explain_dnd;
    GtkLabel    *lbl_recv_title;
    GtkLabel    *lbl_recv_contents;

    Ftc::Core::GlobalStruct::FtcRequest *last_recv_request;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomeDndSubPage, ftc_home_dnd_sub_page, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_dnd_sub_page_call_update_send_sub_page(FtcHomeDndSubPage *page);

static void ftc_home_dnd_sub_page_call_send_list_add_file(FtcHomeDndSubPage *page, const char *path);

static void ftc_home_dnd_sub_page_set_drag_and_drop(FtcHomeDndSubPage *page);

static void ftc_home_dnd_sub_page_received_drag_and_drop_data (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *sel_data, guint info, guint time, gpointer data);

static void ftc_home_dnd_sub_page_on_button_press_event_box_dnd_area(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_home_dnd_sub_page_on_button_press_event_box_message_mode(GtkWidget *widget, GdkEvent *event, gpointer data);

static void ftc_home_dnd_sub_page_file_select(FtcHomeDndSubPage *page);

static void ftc_home_dnd_sub_page_on_button_press_event_box_batch_download(GtkWidget *widget, GdkEvent *event, gpointer data);


///////////////////////////////////////////////////////////////////

static void
ftc_home_dnd_sub_page_init (FtcHomeDndSubPage *page)
{
    FtcHomeDndSubPagePrivate *priv = NULL;
    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private (page);

    //  Todo..
    //  Signal (Event) 함수 연결
    //  box_dnd_area(event_box_dnd_area) click
    g_signal_connect(priv->event_box_dnd_area, "button-press-event", G_CALLBACK(ftc_home_dnd_sub_page_on_button_press_event_box_dnd_area), page);

    //  img_message_mode(event_box_message_mode) click
    g_signal_connect(priv->event_box_message_mode, "button-press-event", G_CALLBACK(ftc_home_dnd_sub_page_on_button_press_event_box_message_mode), page);

    //  imgbtn_batch_download(event_box_batch_download) click
    g_signal_connect(priv->event_box_batch_download, "button-press-event", G_CALLBACK(ftc_home_dnd_sub_page_on_button_press_event_box_batch_download), page);

    //  Drag and Drop 설정
    ftc_home_dnd_sub_page_set_drag_and_drop(page);

    //  크기 설정
    gtk_widget_set_size_request(GTK_WIDGET(priv->box_recv_info), 400, 72);
    gtk_widget_set_size_request(GTK_WIDGET(priv->stack_header_recv), 400, 53);
    gtk_widget_set_size_request(GTK_WIDGET(priv->event_box_dnd_area), 400, 415);
    
    //  디자인 설정
    //  CSS 설정
    gtk_widget_set_name(GTK_WIDGET(page), "default_background_page");
    gtk_widget_set_name(GTK_WIDGET(priv->event_box_dnd_area), "box_background_white");
    gtk_widget_set_name(GTK_WIDGET(priv->box_recv_info), "box_background_white");
    gtk_widget_set_name(GTK_WIDGET(priv->box_dnd_border), "box_border");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_explain_dnd), "label_dnd_explain");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_title), "label_recv_info_title");
    if (ftc_ui_is_internal_location()) {
        //  업무 망 
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_contents), "label_recv_info_contents_is");
    } else {
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_recv_contents), "label_recv_info_contents_es");
    }

    //  버튼 이미지 설정
    gtk_image_set_from_resource(priv->img_file_add, "/com/hunesion/ftcapp/ADD-ICON-16PX.png");
    gtk_image_set_from_resource(priv->img_message_mode, "/com/hunesion/ftcapp/SNS-ICON-24PX.png");
    if (ftc_ui_is_internal_location()) {
        //  업무 망
        gtk_image_set_from_resource(priv->imgbtn_batch_download, "/com/hunesion/ftcapp/DOWNLOAD-ICON-IS.png");
    } else {
        //  인터넷 망
        gtk_image_set_from_resource(priv->imgbtn_batch_download, "/com/hunesion/ftcapp/DOWNLOAD-ICON-ES.png");
    }
    
    
    //  Background 색상 설정

    //  기타 CSS 설정
    

}

static void
ftc_home_dnd_sub_page_dispose (GObject *object)
{
    FtcHomeDndSubPagePrivate *priv;
    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private(FTC_HOME_DND_SUB_PAGE(object));

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);
    
    G_OBJECT_CLASS(ftc_home_dnd_sub_page_parent_class)->dispose(object);
}

static void
ftc_home_dnd_sub_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_home_dnd_sub_page_parent_class)->finalize(object);
}

static void
ftc_home_dnd_sub_page_class_init (FtcHomeDndSubPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_dnd_sub_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_dnd_sub_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-dnd-sub-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, img_file_add);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, img_message_mode);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, imgbtn_batch_download);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, lbl_explain_dnd);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, lbl_recv_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, lbl_recv_contents);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, event_box_dnd_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, event_box_message_mode);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, event_box_batch_download);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, box_dnd_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, box_recv_info);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, box_dnd_border);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeDndSubPage, stack_header_recv);
}

FtcHomeDndSubPage *
ftc_home_dnd_sub_page_new (FtcHomePage *home_page)
{
    FtcHomeDndSubPage *page = FTC_HOME_DND_SUB_PAGE(g_object_new (FTC_HOME_DND_SUB_PAGE_TYPE, NULL));
    FtcHomeDndSubPagePrivate *priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private (page);

    priv->home_page = home_page;

    //  Recv Header 설정
    priv->header_recv_box = ftc_home_header_recv_box_new(priv->home_page, FTC_VIEW_HOME_SUB_PAGE_RECV_LIST);
    gtk_stack_add_named(priv->stack_header_recv, GTK_WIDGET(priv->header_recv_box), FTC_VIEW_HOME_HEADER_RECV);
    gtk_stack_set_visible_child_name(priv->stack_header_recv, FTC_VIEW_HOME_HEADER_RECV);

    return page;
}

static void 
ftc_home_dnd_sub_page_call_update_send_sub_page(FtcHomeDndSubPage *page)
{
    FtcHomeDndSubPagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    UpdatePageEventParam update_page_evt_param;    

    if (! FTC_IS_HOME_DND_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (! event_mgr) {
        return;
    }

    update_page_evt_param.ftc_object = (gpointer)priv->home_page;
    update_page_evt_param.page_name = FTC_VIEW_HOME_SUB_PAGE_SEND_LIST;

    event_mgr->dispatchEvent(FTC_UI_UPDATE_HOME_STACK_EVENT, &update_page_evt_param);
}

static void 
ftc_home_dnd_sub_page_call_send_list_add_file(FtcHomeDndSubPage *page, const char *path)
{
    FtcHomeDndSubPagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    SendListEventParam send_list_evt_param;

    if (! FTC_IS_HOME_DND_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (! event_mgr) {
        return;
    }

    if (! path) {
        return;
    }

    send_list_evt_param.home_page = priv->home_page;
    send_list_evt_param.file_path = path;

    event_mgr->dispatchEvent(FTC_UI_SEND_LIST_ADD_FILE, &send_list_evt_param);
}

static void 
ftc_home_dnd_sub_page_set_drag_and_drop(FtcHomeDndSubPage *page)
{
    if (! FTC_IS_HOME_DND_SUB_PAGE(page)) {
        return;
    }

    gtk_drag_dest_set(GTK_WIDGET(page), GtkDestDefaults::GTK_DEST_DEFAULT_ALL, entry_file_drag, 1, GdkDragAction::GDK_ACTION_COPY);
    g_signal_connect(GTK_WIDGET(page), "drag-data-received", G_CALLBACK(ftc_home_dnd_sub_page_received_drag_and_drop_data), page);
}

static void 
ftc_home_dnd_sub_page_received_drag_and_drop_data (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *sel_data, guint info, guint time, gpointer data)
{
    FtcHomeDndSubPage *page = NULL;
    FtcHomeDndSubPagePrivate *priv = NULL;
    char** uris = NULL;
    std::string path;

    if (! FTC_IS_HOME_DND_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_DND_SUB_PAGE(data);
    
    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    uris = gtk_selection_data_get_uris(sel_data);

    if (! uris) {
        return;
    }

    for (int i = 0; uris[i] != NULL; i++) {
        path = uris[i];
        Ftc::Core::StringUtils::replace(path, "file://", "");
        path = url_decode(path);
        FTC_LOG_DEBUG("dnd filename %d : %s ", i + 1, path.c_str());

        ftc_home_dnd_sub_page_call_send_list_add_file(page, path.c_str());
    }

    ftc_home_dnd_sub_page_call_update_send_sub_page(page);

    if (uris) {
        g_strfreev(uris);
    }
}

static void 
ftc_home_dnd_sub_page_on_button_press_event_box_dnd_area(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeDndSubPage *page = NULL;
    GdkEventButton *event_button = NULL;

    if (! FTC_IS_HOME_DND_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_DND_SUB_PAGE(data);

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        ftc_home_dnd_sub_page_file_select(page);
    }
}

static void 
ftc_home_dnd_sub_page_on_button_press_event_box_message_mode(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeDndSubPage *page = NULL;
    GdkEventButton *event_button = NULL;

    if (! FTC_IS_HOME_DND_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_DND_SUB_PAGE(data);

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        ftc_home_dnd_sub_page_call_update_send_sub_page(page);
    }
}

static void ftc_home_dnd_sub_page_file_select(FtcHomeDndSubPage *page)
{
    FtcHomeDndSubPagePrivate *priv = NULL;
    GtkWidget *window = NULL; 
    GtkWidget *file_chooser_dlg = NULL;
    GtkFileChooser *file_chooser = NULL;

    if (! FTC_IS_HOME_DND_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    window = GTK_WIDGET(ftc_ui_get_main_window());
    if (! GTK_IS_WINDOW(window)) {
        return;
    }

    file_chooser_dlg = gtk_file_chooser_dialog_new("파일 및 디렉토리 선택", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN,
     "취소", GTK_RESPONSE_CANCEL, "열기", GTK_RESPONSE_ACCEPT, NULL);
    if (! file_chooser_dlg) {
        return;
    }
    file_chooser = GTK_FILE_CHOOSER(file_chooser_dlg);
    if (! file_chooser) {
        return;
    }

    gtk_file_chooser_set_select_multiple(file_chooser, true);

    if (gtk_dialog_run(GTK_DIALOG(file_chooser_dlg)) == GTK_RESPONSE_ACCEPT) {
        GSList *list = gtk_file_chooser_get_filenames(file_chooser);
        GSList *it = NULL;
        int lst_count = 0;

        if (! list) {
            return;
        }

        it = list;
        lst_count = g_slist_length(list);

        for (int i = 0; i < lst_count; i++) {
            char *file_path = (char*)it->data;
            if (! file_path) {
                return;
            }

            FTC_LOG_DEBUG("file chooser filename %d : %s ", i + 1, file_path);

            ftc_home_dnd_sub_page_call_send_list_add_file(page, file_path);
            
            it = it->next;
        }

        ftc_home_dnd_sub_page_call_update_send_sub_page(page);

        if (list) {
            g_slist_free(list);
            list = NULL;
        }
    }

    if (file_chooser_dlg) {
        gtk_widget_destroy (file_chooser_dlg);
        file_chooser_dlg = NULL;
    }
}

void                
ftc_home_dnd_sub_page_update_last_request(FtcHomeDndSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request)
{
    FtcHomeDndSubPagePrivate *priv = NULL;
    std::string title_show, content = "";

    if (! FTC_IS_HOME_DND_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! ftc_request) {
        return;
    }

    title_show = ftc_ui_reduce_text_pixel(GTK_WIDGET(priv->lbl_recv_title), ftc_request->request_title.c_str(), 300);
    gtk_label_set_text(priv->lbl_recv_title, title_show.c_str());
    if (title_show != ftc_request->request_title) {
        gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_recv_title), ftc_request->request_title.c_str());
    }

    if (ftc_request->file_cnt > 0) {
        content = ftc_request->firstFileName;
        if (ftc_request->file_cnt > 1) {
            content += " 외 " + std::to_string(ftc_request->file_cnt - 1)  + "개";
        }

        content += "; " + ftc_ui_get_file_size_format(ftc_request->totalFileSize);
        gtk_label_set_text(priv->lbl_recv_contents, content.c_str());

        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_batch_download), true);
    } else {
        gtk_label_set_text(priv->lbl_recv_contents, ftc_ui_get_file_size_format(ftc_request->totalFileSize).c_str());

        gtk_widget_set_visible(GTK_WIDGET(priv->event_box_batch_download), false);
    }

    priv->last_recv_request = ftc_request;
}

static void 
ftc_home_dnd_sub_page_on_button_press_event_box_batch_download(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeDndSubPage *page = NULL;
    FtcHomeDndSubPagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    RecvRequestEventParam param;

    if (! FTC_IS_HOME_DND_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_DND_SUB_PAGE(data);

    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        event_mgr = Ftc::Core::EventManager::getInstance();
        if (! event_mgr) {
            return;
        }

        param.ftc_object = priv->home_page;
        param.ftc_request = priv->last_recv_request;

        event_mgr->dispatchEvent(FTC_UI_RECV_DOWNLOAD_ALL, &param);
    }
}

void 
ftc_home_dnd_sub_page_set_new_recv_request_count(FtcHomeDndSubPage *page, int new_count)
{
    FtcHomeDndSubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_DND_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeDndSubPagePrivate*)ftc_home_dnd_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (new_count < 0) {
        return;
    }

    ftc_home_header_recv_box_set_new_recv_request_count(priv->header_recv_box, new_count);
}