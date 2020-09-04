#include "ftchomesendlistrow.h"
#include "ftchomesendlistsubpage.h"
#include "ftcmainwindow.h"
#include "../core/model/UploadOption.h"
#include "../core/StringUtils.h"
#include "../core/TimeUtils.h"
#include "../core/model/PackFilePop.h"
#include "../core/urlencode.h"
#include "../core/FileUtils.h"
#include <sstream>
#include <gdk/gdkkeysyms-compat.h>

struct _FtcHomeSendListSubPage
{
    GtkBox parent;
};

typedef struct _FtcHomeSendListSubPagePrivate FtcHomeSendListSubPagePrivate;

struct _FtcHomeSendListSubPagePrivate
{
    /* Parent에서 정의 */
    FtcHomePage *home_page;

    /* 소스에서 정의 */
    Ftc::Core::PackFilePop *pack_file_pop;
    Ftc::Core::Transfer::Upload *upload;
    Ftc::Core::UploadOption *uploading_option;
    bool is_uploading;
    bool addition_approval_success;
    
    /* 템플릿에서 정의  */
    GtkScrolledWindow *scrolled_window;
    GtkListBox  *listbox_send;
    GtkBox      *box_header_button;
    GtkButton   *btn_send_cancel;
    GtkButton   *btn_send_apply;
    GtkLabel    *lbl_list_summary;
    GtkImage    *img_list_summary;
    GtkBox      *box_info_area;
    GtkEventBox *event_box_send_detail;
    GtkImage    *imgbtn_send_detail;
    GtkEntry    *etr_send_title;
    GtkProgressBar *pbar_progress;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomeSendListSubPage, ftc_home_send_list_sub_page, GTK_TYPE_BOX)

typedef struct _CompleteCheckParam {
    int total_count;
    int complete_count;
} CompleteCheckParam;

typedef struct _UpdateResultStatusParam {
    std::string file_path;
    std::string check_result_param;
    std::string result_info_list_param;
    std::string str_file_param;
    bool is_private_info;
    bool is_approval_line_extension_check;
    Ftc::Core::Transfer::UploadStatus status;

    _UpdateResultStatusParam() {
        is_private_info = false;
        is_approval_line_extension_check = false;
        status = Ftc::Core::Transfer::UploadStatus::UPLOAD_NONE;
    }
} UpdateResultStatusParam;

typedef struct _UpdateProgressParam {
    std::string file_path;
    int progress;
} UpdateProgressParam;

static std::mutex s_mtx;
static std::condition_variable s_cond;

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_send_list_sub_page_add_file(FtcHomeSendListSubPage *page, Ftc::Core::FileInfo &fi, const char *root_dir);     

static void ftc_home_send_list_sub_page_add_directory(FtcHomeSendListSubPage *page, Ftc::Core::FileInfo &fi, const char *root_dir);

static void ftc_home_send_list_sub_page_set_drag_and_drop(FtcHomeSendListSubPage *page);

static void ftc_home_send_list_sub_page_received_drag_and_drop_data (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *sel_data, guint info, guint time, gpointer data);

static void ftc_home_send_list_sub_page_on_click_btn_send_cancel(GtkWidget *widget, gpointer data);

static void ftc_home_send_list_sub_page_on_click_btn_send_apply(GtkWidget *widget, gpointer data);

static void ftc_home_send_list_sub_page_on_button_press_event_box_send_detail(GtkWidget *widget, GdkEvent *event, gpointer data);

static gboolean ftc_home_send_list_sub_page_on_key_press_list_box_send(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

static void ftc_home_send_list_sub_page_list_box_update_header(GtkListBoxRow *row, GtkListBoxRow *before, gpointer data);

static void ftc_home_send_list_sub_page_list_box_row_destroy_notify(gpointer data);

static void ftc_home_send_list_sub_page_to_page(FtcHomeSendListSubPage *page, const char *to_page);

static bool ftc_home_send_list_sub_page_validate_input(FtcHomeSendListSubPage *page, UploadOption *upload_option);

static void ftc_home_send_list_sub_page_update_extension_btn_image(FtcHomeSendListSubPage *page);

static bool ftc_home_send_list_sub_page_get_pack_file_pop(FtcHomeSendListSubPage *page);

static void ftc_home_send_list_sub_page_list_box_clear(FtcHomeSendListSubPage *page);

static void ftc_home_send_list_sub_page_list_box_clear_callback(GtkWidget *widget, gpointer data);

static void ftc_home_send_list_sub_page_list_box_get_size_callback(GtkWidget *widget, gpointer data);

static int  ftc_home_send_list_sub_page_get_total_file_size(FtcHomeSendListSubPage *page);

static void ftc_home_send_list_sub_page_get_total_file_size_callback(GtkWidget *widget, gpointer data);

static std::list<Ftc::Core::FileInfo> ftc_home_send_list_sub_page_get_file_list(FtcHomeSendListSubPage *page);

static void ftc_home_send_list_sub_page_get_file_list_callback(GtkWidget *widget, gpointer data);

static bool ftc_home_send_list_sub_page_start_file_upload(FtcHomeSendListSubPage *page, std::list<Ftc::Core::FileInfo> &list_file_info, Ftc::Core::UploadOption *upload_option);

static void ftc_home_send_list_sub_page_upload_result_event(Ftc::Core::Event *evt);

static void ftc_home_send_list_sub_page_update_activation_view(FtcHomeSendListSubPage *page, bool is_uploading, std::list<Ftc::Core::FileInfo> *file_list);

static bool ftc_home_send_list_sub_page_is_complete_file_upload(FtcHomeSendListSubPage *page);

static CompleteCheckParam ftc_home_send_list_sub_page_get_check_complete_count(FtcHomeSendListSubPage *page);

static void ftc_home_send_list_sub_page_get_check_complete_count_callback(GtkWidget *widget, gpointer data);

static bool ftc_home_send_list_sub_page_complete_file_upload(FtcHomeSendListSubPage *page);

static void ftc_home_send_list_sub_page_upload_complete(FtcHomeSendListSubPage *page, bool is_success, const char *msg);

static void ftc_home_send_list_sub_page_update_file_uploading_progress(FtcHomeSendListSubPage *page, JsonNode *node);

static void ftc_home_send_list_sub_page_update_file_uploading_progress_callback(GtkWidget *widget, gpointer data);

static void ftc_home_send_list_sub_page_update_total_file_uploading_progress(FtcHomeSendListSubPage *page, int uploading_complete_count, int total_file_count);

static void ftc_home_send_list_sub_page_update_total_file_uploading_progress_text(FtcHomeSendListSubPage *page, JsonNode *node);

static void ftc_home_send_list_sub_page_get_uploading_complete_count(FtcHomeSendListSubPage *page, CompleteCheckParam *param);

static void ftc_home_send_list_sub_page_get_uploading_complete_count_callback(GtkWidget *widget, gpointer data);

static void ftc_home_send_list_sub_page_set_list_row_progress_visible(FtcHomeSendListSubPage *page, bool is_visible);

static void ftc_home_send_list_sub_page_set_list_row_progress_visible_callback(GtkWidget *widget, gpointer data);

static void ftc_home_send_list_sub_page_update_vscroll_adjustment(FtcHomeSendListSubPage *page, int uploading_complete_count);

static void ftc_home_send_list_sub_page_set_total_progress_text(FtcHomeSendListSubPage *page, const char *msg);

static void ftc_home_send_list_sub_page_remove_selected_file(FtcHomeSendListSubPage *page);

static void ftc_home_send_list_sub_page_update_file_count_and_size(FtcHomeSendListSubPage *page);

///////////////////////////////////////////////////////////////////

static void
ftc_home_send_list_sub_page_init (FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    GtkCellRenderer *renderer = NULL;
    GtkTreeIter iter;
    Ftc::Core::EventManager *event_mgr = NULL;

    gtk_widget_init_template (GTK_WIDGET (page));
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private (page);

    //  PackFile 클래스 생성
    priv->pack_file_pop = new Ftc::Core::PackFilePop(Ftc::Core::GlobalVar::getLoginLocation().getNetworkId());

    //  업로드 클래스 인스턴스 받고 이벤트 설정
    priv->upload = Ftc::Core::Transfer::Upload::getInstance();
    priv->upload->setResultEvent(FTC_CORE_UPLOAD_RESULT_EVENT, page);

    //  사이즈 설정
    gtk_widget_set_size_request(GTK_WIDGET(priv->btn_send_apply), 114, 40);
    gtk_widget_set_size_request(GTK_WIDGET(priv->btn_send_cancel), 114, 40);

    //  Signal (Event) 함수 연결
    //  btn_send_cancel click
    g_signal_connect(priv->btn_send_cancel, "clicked", G_CALLBACK(ftc_home_send_list_sub_page_on_click_btn_send_cancel), page);
    //  btn_send_apply click
    g_signal_connect(priv->btn_send_apply, "clicked", G_CALLBACK(ftc_home_send_list_sub_page_on_click_btn_send_apply), page);
    //  imgbtn_send_detail (event_box_send_detail) click
    g_signal_connect(priv->event_box_send_detail, "button-press-event", G_CALLBACK(ftc_home_send_list_sub_page_on_button_press_event_box_send_detail), page);
    //  keyboard
    g_signal_connect(priv->listbox_send, "key_press_event", G_CALLBACK(ftc_home_send_list_sub_page_on_key_press_list_box_send), page);

    //  listbox header update
    gtk_list_box_set_header_func(priv->listbox_send, ftc_home_send_list_sub_page_list_box_update_header, page, ftc_home_send_list_sub_page_list_box_row_destroy_notify);

    //  Drag and Drop
    ftc_home_send_list_sub_page_set_drag_and_drop(page);

    //  디자인
    //  Widget CSS 설정
    gtk_widget_set_name(GTK_WIDGET(page), "default_background_page");
    gtk_widget_set_name(GTK_WIDGET(priv->listbox_send), "box_background_white");
    if (ftc_ui_is_internal_location()) {
        gtk_widget_set_name(GTK_WIDGET(priv->btn_send_apply), "button_is");                
    } else {
        gtk_widget_set_name(GTK_WIDGET(priv->btn_send_apply), "button_es");
    }
    gtk_widget_set_name(GTK_WIDGET(priv->btn_send_cancel), "button_send_cancel");
    gtk_widget_set_name(GTK_WIDGET(priv->box_info_area), "box_background_white");
    gtk_widget_set_name(GTK_WIDGET(priv->etr_send_title), "entry_default");
    gtk_widget_set_name(GTK_WIDGET(priv->pbar_progress), "progressbar_total_upload");

    //  플레이스 홀더
    gtk_entry_set_placeholder_text(priv->etr_send_title, "제목");
    
    //  프로그레스바 기본 envisible
    gtk_widget_set_visible(GTK_WIDGET(priv->pbar_progress), false);
    gtk_progress_bar_set_show_text(priv->pbar_progress, true);
    gtk_progress_bar_set_text(priv->pbar_progress, "");

    //  이미지 설정
    gtk_image_set_from_resource(priv->img_list_summary, "/com/hunesion/ftcapp/LIST02-ICON-16PX.png");
    ftc_home_send_list_sub_page_update_extension_btn_image(page);

    //  ListBox
    gtk_list_box_set_selection_mode(priv->listbox_send, GTK_SELECTION_SINGLE);

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (event_mgr) {
        event_mgr->addEventListener(page, FTC_CORE_UPLOAD_RESULT_EVENT, HUNE_CALLBACK_CLS_STATIC_1(ftc_home_send_list_sub_page_upload_result_event));
    }

    priv->is_uploading = false;
}

static void
ftc_home_send_list_sub_page_dispose (GObject *object)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(FTC_HOME_SEND_LIST_SUB_PAGE(object));

    if (priv->pack_file_pop) {
        delete priv->pack_file_pop;
        priv->pack_file_pop = NULL;
    }

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);
    
    G_OBJECT_CLASS(ftc_home_send_list_sub_page_parent_class)->dispose(object);
}

static void
ftc_home_send_list_sub_page_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_home_send_list_sub_page_parent_class)->finalize(object);
}

static void
ftc_home_send_list_sub_page_class_init (FtcHomeSendListSubPageClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_send_list_sub_page_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_send_list_sub_page_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-send-list-sub-page.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, scrolled_window);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, listbox_send);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, box_header_button);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, btn_send_cancel);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, btn_send_apply);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, lbl_list_summary);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, img_list_summary);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, box_info_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, event_box_send_detail);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, imgbtn_send_detail);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, etr_send_title);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListSubPage, pbar_progress);
    
}

FtcHomeSendListSubPage *
ftc_home_send_list_sub_page_new (FtcHomePage *home_page)
{
    FtcHomeSendListSubPage *page = FTC_HOME_SEND_LIST_SUB_PAGE(g_object_new (FTC_HOME_SEND_LIST_SUB_PAGE_TYPE, NULL));
    FtcHomeSendListSubPagePrivate *priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private (page);

    priv->home_page = home_page;

    return page;
}

bool                        
ftc_home_send_list_sub_page_enable(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    FtcHomeSendListRow *list_box_row = NULL;
    bool rv = false;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return rv;
    }
    
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private (page);
    if (! priv) {
        return rv;
    }

    //  Init Pack File
    //
    if (ftc_home_send_list_sub_page_get_pack_file_pop(page) == false) {
        return rv;
    }

    ftc_home_send_list_sub_page_activate_extension_window(page, true);
    
    //  InfoBox에 타이틀을 기본으로 첫번째 파일 명으로 설정
    //
    if (ftc_home_send_list_sub_page_list_box_get_size(page) > 0) {
        //  일반 파일 전송 모드 호출 시
        list_box_row = FTC_HOME_SEND_LIST_ROW(gtk_bin_get_child(GTK_BIN(gtk_list_box_get_row_at_index(priv->listbox_send, 0))));
        if (! FTC_IS_HOME_SEND_LIST_ROW(list_box_row)) {
            return rv;
        }

        gtk_entry_set_text(priv->etr_send_title, ftc_home_send_list_row_get_file_name(list_box_row));
    } else {
        //  메세지 모드 호출 시
        ftc_home_send_list_sub_page_update_file_count_and_size(page);
    }

    rv = true;
    return rv;
}

void                        
ftc_home_send_list_sub_page_disable(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    ftc_home_send_list_sub_page_list_box_clear(page);
    ftc_home_send_list_sub_page_update_extension_btn_image(page);

    priv->pack_file_pop->clear();
    priv->uploading_option = NULL;
    gtk_entry_set_text(priv->etr_send_title, "");
}

void                        
ftc_home_send_list_sub_page_add_list(FtcHomeSendListSubPage *page, const char *path)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    Ftc::Core::FileInfo fi;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    if (! path) {
        return;
    }

    fi = Ftc::Core::FileInfo::getFileInfo(path);

    ftc_home_send_list_sub_page_add_list_file_info(page, fi);
}

void                        
ftc_home_send_list_sub_page_add_list_file_info(FtcHomeSendListSubPage *page, Ftc::Core::FileInfo &fi)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    if (fi.isDirectory()) {
        ftc_home_send_list_sub_page_add_directory(page, fi, fi.getFullPath().c_str());
    } else if (fi.isFile()) {
        ftc_home_send_list_sub_page_add_file(page, fi, NULL);
    }
}

static void                        
ftc_home_send_list_sub_page_add_file(FtcHomeSendListSubPage *page, Ftc::Core::FileInfo &fi, const char *root_dir)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::string str_root_dir, relative_dir, tmp;
    int index = 0;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    if (fi.isExist()) {
        if (fi.isFile()) {
            std::list<Ftc::Core::FileInfo> lst_fi;

            if (root_dir) {
                str_root_dir = root_dir;
                relative_dir = fi.getPath();

                index = str_root_dir.rfind("/");
                tmp = str_root_dir.substr(0, index + 1); 
                Ftc::Core::StringUtils::replace(relative_dir, tmp, "");

                fi.setRelativeDir(relative_dir.c_str());
            }

            lst_fi = ftc_home_send_list_sub_page_get_file_list(page);

            for (auto &it : lst_fi) {
                if (it.getFullPath() == fi.getFullPath()) {
                    if (it.getRelativeDir() == fi.getRelativeDir()) {
                        FTC_LOG_DEBUG("이미 추가된 파일!! %s", fi.getFullPath().c_str());
                        return;
                    }
                }
            }

            GtkWidget *row = GTK_WIDGET(ftc_home_send_list_row_new(fi));
            gtk_list_box_insert(priv->listbox_send, row, -1);
        }
    }
}

static void                        
ftc_home_send_list_sub_page_add_directory(FtcHomeSendListSubPage *page, Ftc::Core::FileInfo &fi, const char *root_dir)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::string dir_path;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private (page);
    if (! priv) {
        return;
    }

    dir_path = fi.getFullPath();

    if (fi.isExist()) {
        if (fi.isDirectory()) {
            DIR *dir = NULL;
            struct dirent *de = NULL;

            dir = opendir(dir_path.c_str());
            if (dir) {
                while (de = readdir(dir)) {
                    std::string full_path = dir_path + "/" + de->d_name;
                    FTC_LOG_DEBUG("%s ", full_path.c_str());

                    Ftc::Core::FileInfo subFi = Ftc::Core::FileInfo::getFileInfo(full_path.c_str());

                    if (subFi.isExist()) {
                        if (subFi.isDirectory()) {
                            if (strcmp(de->d_name, ".") == 0) {
                                continue;
                            } else if (strcmp(de->d_name, "..") == 0) {
                                continue;
                            } else {
                                ftc_home_send_list_sub_page_add_directory(page, subFi, root_dir);
                            }
                        } else if (subFi.isFile()) {
                            ftc_home_send_list_sub_page_add_file(page, subFi, root_dir);
                        }
                    }
                }

                closedir(dir);
            }
        }
    }  
}

static void 
ftc_home_send_list_sub_page_set_drag_and_drop(FtcHomeSendListSubPage *page)
{
    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    gtk_drag_dest_set(GTK_WIDGET(page), GtkDestDefaults::GTK_DEST_DEFAULT_ALL, entry_file_drag, 1, GdkDragAction::GDK_ACTION_COPY);
    g_signal_connect(GTK_WIDGET(page), "drag-data-received", G_CALLBACK(ftc_home_send_list_sub_page_received_drag_and_drop_data), page);
}

static void 
ftc_home_send_list_sub_page_received_drag_and_drop_data (GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *sel_data, guint info, guint time, gpointer data)
{
    FtcHomeSendListSubPage *page = NULL;
    FtcHomeSendListSubPagePrivate *priv = NULL;
    char** uris = NULL;
    std::string path;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(data)) {
        return;
    }

    page = FTC_HOME_SEND_LIST_SUB_PAGE(data);

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! sel_data) {
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

        ftc_home_send_list_sub_page_add_list(page, path.c_str());
    }

    if (uris) {
        g_strfreev(uris);
    }
}

static void 
ftc_home_send_list_sub_page_on_click_btn_send_cancel(GtkWidget *widget, gpointer data)
{
    FtcHomeSendListSubPage *page = NULL;
    FtcHomeSendListSubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_SEND_LIST_SUB_PAGE(data);

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (priv->is_uploading) {
        //  업로드 중이면 업로드를 취소한다.
        priv->upload->cancel();
        ftc_home_send_list_sub_page_upload_complete(page, false, NULL);
    } else {
        ftc_home_send_list_sub_page_to_page(page, FTC_VIEW_HOME_SUB_PAGE_DND);
    }
}

static void 
ftc_home_send_list_sub_page_on_click_btn_send_apply(GtkWidget *widget, gpointer data)
{
    FtcHomeSendListSubPage *page = NULL;
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::list<Ftc::Core::FileInfo> list_file_info; 
    Ftc::Core::UploadOption *upload_option = NULL; 
    std::string error;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_SEND_LIST_SUB_PAGE(data);

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (priv->is_uploading) {
        return;
    }

    list_file_info = ftc_home_send_list_sub_page_get_file_list(page);

    upload_option->setSendTitle(gtk_entry_get_text(priv->etr_send_title));

    if (ftc_home_send_list_sub_page_validate_input(page, upload_option) == false) {
        return;
    }

    //  업로드 옵션 저장
    priv->uploading_option = upload_option;
    priv->is_uploading = true;

    //  업로드 시작
    //
    if (ftc_home_send_list_sub_page_start_file_upload(page, list_file_info, upload_option) == true) {
        ftc_home_send_list_sub_page_update_activation_view(page, priv->is_uploading, &list_file_info);
    } else {
        priv->uploading_option = NULL;
        priv->is_uploading = false;
    }

}

static void 
ftc_home_send_list_sub_page_on_button_press_event_box_send_detail(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    FtcHomeSendListSubPage *page = NULL;
    FtcHomeSendListSubPagePrivate *priv = NULL;
    GdkEventButton *event_button = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_SEND_LIST_SUB_PAGE(data);

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! event) {
        return;
    }
    event_button = (GdkEventButton*)event;

    if (event_button->type == GDK_BUTTON_PRESS/*one clicked*/ && event_button->button == 1/*left clicked*/)
    {
        ftc_home_send_list_sub_page_activate_extension_window(page, false);
    }
}

static gboolean
ftc_home_send_list_sub_page_on_key_press_list_box_send(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    FtcHomeSendListSubPage *page = NULL;
    FtcHomeSendListSubPagePrivate *priv = NULL;
    Ftc::Core::Transfer::Upload *upload = Ftc::Core::Transfer::Upload::getInstance();
    gboolean rv = false;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(user_data)) {
        return rv;
    }
    page = FTC_HOME_SEND_LIST_SUB_PAGE(user_data);

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    if (! widget || ! event) {
        return rv;
    }

    if (! upload) {
        return rv;
    }

    if (upload->isWorking()) {
        return rv;
    }

    switch (event->keyval) {
        case GDK_Delete:
        {
            ftc_home_send_list_sub_page_remove_selected_file(page);
        }   
    }
}

static void 
ftc_home_send_list_sub_page_list_box_update_header(GtkListBoxRow *row, GtkListBoxRow *before, gpointer data)
{

    FtcHomeSendListSubPage *page = NULL;
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::string str = "";
    unsigned int list_row_count = 0;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_SEND_LIST_SUB_PAGE(data);

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);

    ftc_home_send_list_sub_page_update_file_count_and_size(page);
}

static void ftc_home_send_list_sub_page_list_box_row_destroy_notify(gpointer data)
{
}

static void 
ftc_home_send_list_sub_page_to_page(FtcHomeSendListSubPage *page, const char *to_page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    UpdatePageEventParam update_page_evt_param;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (!event_mgr) {
        return;
    }

    ftc_home_send_list_sub_page_disable(page);

    update_page_evt_param.ftc_object = (gpointer)priv->home_page;
    update_page_evt_param.page_name = to_page;

    event_mgr->dispatchEvent(FTC_UI_UPDATE_HOME_STACK_EVENT, &update_page_evt_param);
}

static void 
ftc_home_send_list_sub_page_update_extension_btn_image(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);

    //  Extension 버튼 이미지 설정
    if (ftc_ui_is_extension_widnow()) {
        if (ftc_ui_is_internal_location()) {
            //  업무 망
            gtk_image_set_from_resource(priv->imgbtn_send_detail, "/com/hunesion/ftcapp/ARROW-LEFT-IS.png");
        } else {
            //  인터넷 망
            gtk_image_set_from_resource(priv->imgbtn_send_detail, "/com/hunesion/ftcapp/ARROW-LEFT-ES.png");
        }
    } else {
        if (ftc_ui_is_internal_location()) {
            //  업무 망
            gtk_image_set_from_resource(priv->imgbtn_send_detail, "/com/hunesion/ftcapp/ARROW-RIGHT-IS.png");
        } else {
            //  인터넷 망
            gtk_image_set_from_resource(priv->imgbtn_send_detail, "/com/hunesion/ftcapp/ARROW-RIGHT-ES.png");
        }
    }
}

void                        
ftc_home_send_list_sub_page_activate_extension_window(FtcHomeSendListSubPage *page, bool only_extension)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    Ftc::Core::EventManager *event_mgr = NULL;
    UpdatePageEventParam param;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }
    
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);

    event_mgr = Ftc::Core::EventManager::getInstance();
    if (! event_mgr) {
        return;
    }

    param.ftc_object = ftc_ui_get_main_window();
    if (only_extension) {
        param.page_name = FTC_VIEW_WINDOW_PAGE_SEND_DETAIL_ONLY_EXTENSION;
    } else {
        param.page_name = FTC_VIEW_WINDOW_PAGE_SEND_DETAIL;
    }

    event_mgr->dispatchEvent(FTC_UI_UPDATE_MAIN_STACK_EVENT, &param);

    ftc_home_send_list_sub_page_update_extension_btn_image(page);
}

static bool 
ftc_home_send_list_sub_page_validate_input(FtcHomeSendListSubPage *page, UploadOption *upload_option)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::string error_msg;
    bool rv = false;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    if (! upload_option) {
        return rv;
    }
    
    try
    {
        //  네트워크 선택
        if (upload_option->getNetworkSize() == 0) {
            //  네트워크 선택되어 있지 않음
            //
            throw std::runtime_error(error_msg);
        }

        //  수신자 선택
        if (upload_option->getUserSize() == 0) {
            //  보낼 상대가 선택되어 있지 않음
            throw std::runtime_error(error_msg);
        }

        //  제목
        if (strlen(upload_option->getSendTitle()) <= 0) {
            //  제목을 입력해 주세요.
            throw std::runtime_error(error_msg);
        }

        rv = true;
    } catch (const std::runtime_error& e){
        ftc_ui_message_box(GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, e.what());
    }
    
    return rv;
}

static bool 
ftc_home_send_list_sub_page_get_pack_file_pop(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    JsonNode *node = NULL;
    bool rv = false;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    auto resp = Ftc::Core::Request::requestInitPackFilePop();

    rv = true;
    return rv;
}

static void 
ftc_home_send_list_sub_page_list_box_clear(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send), ftc_home_send_list_sub_page_list_box_clear_callback, page);
}

static void 
ftc_home_send_list_sub_page_list_box_clear_callback(GtkWidget *widget, gpointer data)
{
    FtcHomeSendListSubPage *page = NULL;
    FtcHomeSendListSubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(data)) {
        return;
    }
    page = FTC_HOME_SEND_LIST_SUB_PAGE(data);

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! widget) {
        return;
    }

    gtk_container_remove(GTK_CONTAINER(priv->listbox_send), widget);
}

int  
ftc_home_send_list_sub_page_list_box_get_size(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    int list_total_count = 0;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return list_total_count;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return list_total_count;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send), ftc_home_send_list_sub_page_list_box_get_size_callback, &list_total_count);

    return list_total_count;
}

static void 
ftc_home_send_list_sub_page_list_box_get_size_callback(GtkWidget *widget, gpointer data)
{
    int *list_total_count_ptr = NULL;
    if (! data) {
        return;
    }

    list_total_count_ptr = (int*)data;
    (*list_total_count_ptr)++;
}

static int 
ftc_home_send_list_sub_page_get_total_file_size(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    int total_file_size = 0;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return total_file_size;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return total_file_size;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send), ftc_home_send_list_sub_page_get_total_file_size_callback, &total_file_size);

    return total_file_size;
}

static void 
ftc_home_send_list_sub_page_get_total_file_size_callback(GtkWidget *widget, gpointer data)
{
    FtcHomeSendListRow *row = NULL;
    int *total_file_size_ptr = NULL;

    if (! widget || ! data) {
        return;
    }

    row = FTC_HOME_SEND_LIST_ROW(gtk_bin_get_child(GTK_BIN(widget))); 
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }

    total_file_size_ptr = (int*)data;

    (*total_file_size_ptr) += ftc_home_send_list_row_get_file_size(row);
}

static std::list<Ftc::Core::FileInfo> 
ftc_home_send_list_sub_page_get_file_list(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::list<Ftc::Core::FileInfo> list_file;
    if (!FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return list_file;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return list_file;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send), ftc_home_send_list_sub_page_get_file_list_callback, &list_file);

    return list_file;
}

static void 
ftc_home_send_list_sub_page_get_file_list_callback(GtkWidget *widget, gpointer data)
{
    GtkListBoxRow *list_row = NULL;
    FtcHomeSendListRow *row = NULL;
    std::list<Ftc::Core::FileInfo> *list_file = NULL;
    Ftc::Core::FileInfo fi;
    const char *relative_dir = NULL, *file_path = NULL;

    if (! widget || ! data) {
        return;
    }
    list_row = (GtkListBoxRow*)widget;
    list_file = (std::list<Ftc::Core::FileInfo>*)data;

    row = FTC_HOME_SEND_LIST_ROW(gtk_bin_get_child(GTK_BIN(list_row)));
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }

    
    file_path = ftc_home_send_list_row_get_file_path(row);
    if (! file_path) {
        return;
    }
    fi = Ftc::Core::FileInfo::getFileInfo(file_path);

    relative_dir = ftc_home_send_list_row_get_relative_dir(row);
    if (! relative_dir) {
        return;
    }
    fi.setRelativeDir(relative_dir);

    list_file->push_back(fi);
}

static bool 
ftc_home_send_list_sub_page_start_file_upload(FtcHomeSendListSubPage *page, std::list<Ftc::Core::FileInfo> &list_file_info, Ftc::Core::UploadOption *upload_option)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    int network_size = 0;
    std::list<std::string> network_ids;
    bool rv = false;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    if (list_file_info.size() <= 0) {
        return rv;
    }


    if (! upload_option) {
        return rv;
    }

    network_size = upload_option->getNetworkSize();
    if (network_size <= 0) {
        return rv;
    }

    for (int i = 0; i < network_size; i++) {
        Ftc::Core::GlobalStruct::FtcNetwork network = upload_option->getNetwork(i);

        network_ids.push_back(network.to_network_id);
    }

    if (! priv->upload) {
        return rv;
    }
    rv = priv->upload->pushFiles(list_file_info, network_ids);

    return rv;
}

static void 
ftc_home_send_list_sub_page_upload_result_event(Ftc::Core::Event *evt)
{
    FtcHomeSendListSubPage *page = NULL;
    FtcHomeSendListSubPagePrivate *priv = NULL;
    Ftc::Core::Transfer::UploadResultParam *param = NULL;

    if (!evt) {
        return;
    }
    
    if (! evt->getUserData()) {
        return;
    }
    param = (Ftc::Core::Transfer::UploadResultParam*)evt->getUserData();

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(param->user_data)) {
        return;
    } 
    page = FTC_HOME_SEND_LIST_SUB_PAGE(param->user_data);

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (!priv) {
        return;
    }

    if (priv->is_uploading == false) {
        return;
    }

    if (!param->node) {
        return;
    }

    switch (param->status)
    {
        case Ftc::Core::Transfer::UploadStatus::UPLOADING :
            {
                CompleteCheckParam uploading_count;

                //  개별 프로그레스
                ftc_home_send_list_sub_page_update_file_uploading_progress(page, param->node);

                ftc_home_send_list_sub_page_get_uploading_complete_count(page, &uploading_count);
                //  전체 프로그레스
                ftc_home_send_list_sub_page_update_total_file_uploading_progress(page, uploading_count.complete_count, uploading_count.total_count);
                //  리스트박스 오토스크롤
                ftc_home_send_list_sub_page_update_vscroll_adjustment(page, uploading_count.complete_count);
            }
            break;
        case Ftc::Core::Transfer::UploadStatus::UPLOAD_CHECKING :
            {
                //  MSG에 검사 중 메세지
                ftc_home_send_list_sub_page_update_total_file_uploading_progress_text(page, param->node);
            }
            break;
        case Ftc::Core::Transfer::UploadStatus::UPLOAD_COMPLETE :
            {
                //  모든 파일 성공여부 검사
                if (ftc_home_send_list_sub_page_is_complete_file_upload(page) == true) {
                    //  업로드 체크 완료 후 마무리 프로세스
                    ftc_home_send_list_sub_page_complete_file_upload(page);
                }
            }
            break;
        case Ftc::Core::Transfer::UploadStatus::UPLOAD_FAIL :
            {
                std::string error_msg;
                
                //  업로드 취소 처리는 Upload Class에서 알아서 함
                ftc_home_send_list_sub_page_upload_complete(page, false, error_msg.c_str());
            }
            break;
        default:
            break;
    }
}

static void 
ftc_home_send_list_sub_page_update_activation_view(FtcHomeSendListSubPage *page, bool is_uploading, std::list<Ftc::Core::FileInfo> *file_list)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::stringstream ss, ss1, ss2, ss3;
    std::string first_file_name, reduce_file_name;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if ( !priv) {
        return;
    }

    if (is_uploading) {
        //  업로딩 시작
        gtk_widget_set_visible(GTK_WIDGET(priv->box_info_area), false);
        gtk_widget_set_visible(GTK_WIDGET(priv->pbar_progress), true);
        
        if(file_list) {
            first_file_name = file_list->front().getFileName();
            reduce_file_name = ftc_ui_reduce_text_pixel(GTK_WIDGET(priv->lbl_list_summary), first_file_name.c_str(), 270);

            if (file_list->size() > 1) {
                //  외 %d건
                ss << " " + Ftc::Core::StringUtils::format("외 %d건", file_list->size());
            }   
            ss2 << reduce_file_name << ss.str();
            gtk_label_set_text(priv->lbl_list_summary,  ss2.str().c_str());
            if (first_file_name != reduce_file_name) {
                ss3 << first_file_name << ss.str();
                gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_list_summary), ss3.str().c_str());
            }

            ss1 << "0/";
            ss1 << std::to_string(file_list->size());
            ss1 << "(0%)";
            gtk_progress_bar_set_text(priv->pbar_progress, ss1.str().c_str()); 
        }

        ftc_home_send_list_sub_page_set_list_row_progress_visible(page, true);
    } else {
        //  업로딩 종료(혹은 취소)
        gtk_widget_set_visible(GTK_WIDGET(priv->box_info_area), true);
        gtk_widget_set_visible(GTK_WIDGET(priv->pbar_progress), false);
        gtk_progress_bar_set_text(priv->pbar_progress, "");
        gtk_progress_bar_set_fraction(priv->pbar_progress, 0);
        gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_list_summary), "");

        ftc_home_send_list_sub_page_set_list_row_progress_visible(page, false);
    }
}

static bool
ftc_home_send_list_sub_page_is_complete_file_upload(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    CompleteCheckParam param;
    bool rv = false;
    

    if (!FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    param = ftc_home_send_list_sub_page_get_check_complete_count(page);

    if (param.total_count == param.complete_count) {
        rv = true;
    }

    return rv;
}

static CompleteCheckParam 
ftc_home_send_list_sub_page_get_check_complete_count(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    CompleteCheckParam rv;
    
    rv.complete_count = 0;
    rv.total_count = 0;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return rv;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (!priv) {
        return rv;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send), ftc_home_send_list_sub_page_get_check_complete_count_callback, &rv);

    return rv;
}

static void 
ftc_home_send_list_sub_page_get_check_complete_count_callback(GtkWidget *widget, gpointer data)
{
    GtkListBoxRow *list_row = NULL;
    FtcHomeSendListRow *row = NULL;
    CompleteCheckParam *param = NULL;
    Ftc::Core::Transfer::UploadStatus status = Ftc::Core::Transfer::UploadStatus::UPLOAD_NONE;

    if (! widget || ! data) {
        return;
    }

    param = (CompleteCheckParam*)data;
    list_row = (GtkListBoxRow*)widget;

    row = FTC_HOME_SEND_LIST_ROW(gtk_bin_get_child(GTK_BIN(list_row)));
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }

    status = ftc_home_send_list_row_get_upload_status(row);

    if (status == Ftc::Core::Transfer::UploadStatus::UPLOAD_COMPLETE) {
        param->complete_count++;
    }

    param->total_count++;
}


static bool 
ftc_home_send_list_sub_page_complete_file_upload(FtcHomeSendListSubPage *page)
{
    bool rv = false;
    std::string error;
    const char *error_msg = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return rv;
    }
    
    ftc_home_send_list_sub_page_upload_complete(page, true, NULL);
    rv = true;
    return rv;
}

static void 
ftc_home_send_list_sub_page_upload_complete(FtcHomeSendListSubPage *page, bool is_success, const char *msg)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::string str_msg;

    if (!FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (!priv) {
        return;
    }

    priv->is_uploading = false;

    if (msg) {
        str_msg = msg;
        if (str_msg.length() > 0) {
            Ftc::Core::StringUtils::replace(str_msg, "ERROR:", "");
            Ftc::Core::StringUtils::replace(str_msg, "\\n", "\n");
            Ftc::Core::StringUtils::trim(str_msg);

            //  메세지출력
            ftc_ui_message_box(GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, str_msg.c_str());
        }
    }

    //  업로딩 완료 SendSubPage UI처리
    ftc_home_send_list_sub_page_update_activation_view(page, priv->is_uploading, NULL);

    if (false) {
        //  송신이력화면으로 전환 TODO
        ftc_home_send_list_sub_page_to_page(page, FTC_VIEW_HOME_SUB_PAGE_SEND_HISTORY);  
    } else {
        //  메인화면 (DND)으로 전환
        ftc_home_send_list_sub_page_to_page(page, FTC_VIEW_HOME_SUB_PAGE_DND);  
    }

}

static void 
ftc_home_send_list_sub_page_update_file_uploading_progress(FtcHomeSendListSubPage *page, JsonNode *node)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    UpdateProgressParam param;
    std::string file_path;
    int progress = 0;

    if (!FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (!priv) {
        return;
    }

    file_path = Ftc::Core::JsonUtils::getStdString(node, "info:filepath");
    if (file_path.length() <= 0) {
        return;
    }

    progress = Ftc::Core::JsonUtils::getInt(node, "info:progress");
    if (progress <= 0) {
        return;
    }

    param.file_path = file_path;
    param.progress = progress;

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send), ftc_home_send_list_sub_page_update_file_uploading_progress_callback, &param);
}

static void 
ftc_home_send_list_sub_page_update_file_uploading_progress_callback(GtkWidget *widget, gpointer data)
{
    GtkListBoxRow *list_row = NULL;
    FtcHomeSendListRow *row = NULL;
    UpdateProgressParam *param = NULL;
    std::string file_path;

    if (!widget || !data) {
        return;
    }

    param = (UpdateProgressParam*)data;
    list_row = (GtkListBoxRow*)widget;

    row = FTC_HOME_SEND_LIST_ROW(gtk_bin_get_child(GTK_BIN(list_row)));
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }

    file_path = ftc_home_send_list_row_get_file_path(row);
    if (file_path.length() <= 0) {
        return;
    }

    if (param->file_path == file_path) {
        ftc_home_send_list_row_set_progress(row, param->progress);
    }
}

static void 
ftc_home_send_list_sub_page_update_total_file_uploading_progress(FtcHomeSendListSubPage *page, int uploading_complete_count, int total_file_count)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::stringstream ss;
    double cur_progress = 0, progress = 0;

    if (!FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (!priv) {
        return;
    }

    progress = (double)uploading_complete_count / total_file_count;

    if (progress <= 0) {
        return;
    } else if (progress > 1) {
        progress = 1;
    }

    cur_progress = gtk_progress_bar_get_fraction(priv->pbar_progress);
    if (cur_progress < progress) {
        gtk_progress_bar_set_fraction(priv->pbar_progress, progress);
        //  프로그레스 반영
        ss << std::to_string(uploading_complete_count);
        ss << "/";
        ss << std::to_string(total_file_count);
        ss << "(";
        ss << std::to_string((int)(progress * 100));
        ss << "%)";
        gtk_progress_bar_set_text(priv->pbar_progress, ss.str().c_str());
    }
}

static void 
ftc_home_send_list_sub_page_update_total_file_uploading_progress_text(FtcHomeSendListSubPage *page, JsonNode *node)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    const char *msg = NULL;
    
    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! node) {
        return;
    }

    msg = JsonUtils::getString(node, "msg");
    ftc_home_send_list_sub_page_set_total_progress_text(page, msg);

}

static void 
ftc_home_send_list_sub_page_get_uploading_complete_count(FtcHomeSendListSubPage *page, CompleteCheckParam *param)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;

    if (!FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (!priv) {
        return;
    }

    if (!param) {
        return;
    }
    
    param->complete_count = 0;
    param->total_count = 0;

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send), ftc_home_send_list_sub_page_get_uploading_complete_count_callback, param);
}

static void 
ftc_home_send_list_sub_page_get_uploading_complete_count_callback(GtkWidget *widget, gpointer data)
{
    GtkListBoxRow *list_row = NULL;
    FtcHomeSendListRow *row = NULL;
    CompleteCheckParam *param = NULL;
    Ftc::Core::Transfer::UploadStatus status = Ftc::Core::Transfer::UploadStatus::UPLOAD_NONE;

    if (! widget || ! data) {
        return;
    }

    param = (CompleteCheckParam*)data;
    list_row = (GtkListBoxRow*)widget;

    row = FTC_HOME_SEND_LIST_ROW(gtk_bin_get_child(GTK_BIN(list_row)));
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }

    status = ftc_home_send_list_row_get_upload_status(row);

    if (status == Ftc::Core::Transfer::UploadStatus::UPLOADED || status == Ftc::Core::Transfer::UploadStatus::UPLOAD_COMPLETE) {
        param->complete_count++;
    }

    param->total_count++;
}

static void 
ftc_home_send_list_sub_page_set_list_row_progress_visible(FtcHomeSendListSubPage *page, bool is_visible)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(priv->listbox_send), ftc_home_send_list_sub_page_set_list_row_progress_visible_callback, &is_visible);
}

static void 
ftc_home_send_list_sub_page_set_list_row_progress_visible_callback(GtkWidget *widget, gpointer data)
{
    GtkListBoxRow *list_row = NULL;
    FtcHomeSendListRow *row = NULL;
    bool *is_visible = NULL;

    if (!widget || !data) {
        return;
    }

    is_visible = (bool*)data;
    list_row = (GtkListBoxRow*)widget;

    row = FTC_HOME_SEND_LIST_ROW(gtk_bin_get_child(GTK_BIN(list_row)));
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }   

    ftc_home_send_list_row_set_visible_progressbar(row, *is_visible);
}

static void
ftc_home_send_list_sub_page_update_vscroll_adjustment(FtcHomeSendListSubPage *page, int uploading_complete_count) 
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    GtkAdjustment *adj = NULL;
    double update_scroll_value = 0, upper = 0;

    if (uploading_complete_count < 7) {
        return;
    }

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }

    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    adj = gtk_scrolled_window_get_vadjustment(priv->scrolled_window);
    if (! adj) {
        return;
    }
    upper = gtk_adjustment_get_upper(adj);
    
    update_scroll_value = (uploading_complete_count - 6) * 46;
    if (update_scroll_value > upper) {
        update_scroll_value = upper;
    }

    gtk_adjustment_set_value(adj, update_scroll_value);
}


static void
ftc_home_send_list_sub_page_set_total_progress_text(FtcHomeSendListSubPage *page, const char *msg)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::string msg_str, msg_new, msg_before, dot_before;
    int dot_index_new = 0, dot_index_before = 0, dot_len = 0;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    if (! msg) {
        return;
    }

    msg_new = msg;

    dot_index_new = msg_new.find(".");
    if (dot_index_new != std::string::npos) {
        //  메세지의 순수 문장(. 제거)
        msg_new = msg_new.substr(0, dot_index_new);
    }

    msg_before = gtk_progress_bar_get_text(priv->pbar_progress);
    dot_index_before = msg_before.find(".");
    if (dot_index_before != std::string::npos) {
        dot_before = msg_before.substr(dot_index_before);
        //  이전 메세지의 순수 문장(. 제거)
        msg_before = msg_before.substr(0, dot_index_before);
    }
    
    //  .의 개수를 파악해서 붙여준다.
    if (msg_before == msg_new) {
        //  새로 세팅하려는 메세지가 이전 메세지와 같을 시
        dot_len = dot_before.length();
    }

    if (dot_len == 1) {
        msg_str = msg_new + "..";
    } else if (dot_len == 2) {
        msg_str = msg_new + "...";
    } else {
        msg_str = msg_new + ".";
    }

    gtk_progress_bar_set_text(priv->pbar_progress, msg_str.c_str());
}

static void 
ftc_home_send_list_sub_page_remove_selected_file(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    GtkListBoxRow *row = NULL;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    row = gtk_list_box_get_selected_row(priv->listbox_send);
    if (! row) {
        return;
    }

    gtk_container_remove(GTK_CONTAINER(priv->listbox_send), GTK_WIDGET(row));
    ftc_home_send_list_sub_page_update_file_count_and_size(page);
}

static void
ftc_home_send_list_sub_page_update_file_count_and_size(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    std::string str;
    int list_row_count = 0;

    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return;
    }
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return;
    }

    list_row_count = ftc_home_send_list_sub_page_list_box_get_size(page);

    if (list_row_count >= 0) {
        //  lbl_file_count_info
        //  총 %d개 파일, %s
        Ftc::Core::StringUtils::format(str, "총 %d개 파일, %s", list_row_count, ftc_ui_get_file_size_format(ftc_home_send_list_sub_page_get_total_file_size(page)).c_str());
        gtk_label_set_text(priv->lbl_list_summary, str.c_str());
    }
}

bool
ftc_home_send_list_sub_page_is_uploading(FtcHomeSendListSubPage *page)
{
    FtcHomeSendListSubPagePrivate *priv = NULL;
    bool rv = false;
    if (! FTC_IS_HOME_SEND_LIST_SUB_PAGE(page)) {
        return rv;
    }
    priv = (FtcHomeSendListSubPagePrivate*)ftc_home_send_list_sub_page_get_instance_private(page);
    if (! priv) {
        return rv;
    }

    rv = priv->is_uploading;
    return rv;
}
