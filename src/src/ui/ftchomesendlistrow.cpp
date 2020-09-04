#include "ftchomesendlistrow.h"

struct _FtcHomeSendListRow
{
    GtkBox parent;
};

typedef struct _FtcHomeSendListRowPrivate FtcHomeSendListRowPrivate;

struct _FtcHomeSendListRowPrivate
{
    /* 템플릿에서 정의 */
    GtkImage *img_file_icon;
    GtkLabel *lbl_file_name;
    GtkLabel *lbl_file_size;
    GtkLabel *lbl_progress;
    GtkProgressBar *pbar_progress;
    GtkBox *box_progress_area;

    /* 소스코드에서 정의 */
    char        *file_name;
    char        *file_path;
    char        *relative_dir;
    char        *check_result_param;
    char        *result_info_list_param;
    char        *str_file_param;
    long long   file_size;
    bool        is_private_info;
    bool        is_approval_line_extension_check;

    Ftc::Core::Transfer::UploadStatus status;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcHomeSendListRow, ftc_home_send_list_row, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////
static void ftc_home_recv_list_row_on_click_btn_batch_download(GtkWidget *widget, gpointer data);
///////////////////////////////////////////////////////////////////

static void
ftc_home_send_list_row_init (FtcHomeSendListRow *row)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    gtk_widget_init_template (GTK_WIDGET (row));
    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private (row);

    //  Signal (Event) 함수 연결
    
    //  위젯 세팅
    //gtk_level_bar_add_offset_value()
    
    //  디자인 설정
    //  Widget 사이즈
    //gtk_widget_set_size_request(GTK_WIDGET(priv->box_progress_area), 30, -1);

    //  CSS 설정
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_file_name), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_progress), "label_send_progress");
    gtk_widget_set_name(GTK_WIDGET(priv->pbar_progress), "progressbar_individual_upload");
    if (ftc_ui_is_internal_location()) {
        //  업무 망 
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_file_size), "label_recv_info_contents_is");
    } else {
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_file_size), "label_recv_info_contents_es");
    }

    //  현재 상태 입력
    priv->status = Ftc::Core::Transfer::UploadStatus::UPLOAD_NONE;
}

static void
ftc_home_send_list_row_dispose (GObject *object)
{
    FtcHomeSendListRowPrivate *priv;
    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(FTC_HOME_SEND_LIST_ROW(object));

    if (priv->file_name != NULL) {
        g_free(priv->file_name);
        priv->file_name = NULL;
    }

    if (priv->file_path != NULL) {
        g_free(priv->file_path);
        priv->file_path = NULL;
    }

    if (priv->relative_dir != NULL) {
        g_free(priv->relative_dir);
        priv->relative_dir = NULL;
    }

    if (priv->check_result_param != NULL) {
        g_free(priv->check_result_param);
        priv->check_result_param = NULL;
    }

    if (priv->result_info_list_param != NULL) {
        g_free(priv->result_info_list_param);
        priv->result_info_list_param = NULL;
    }

    if (priv->str_file_param != NULL) {
        g_free(priv->str_file_param);
        priv->str_file_param = NULL;
    }

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);

 
    G_OBJECT_CLASS(ftc_home_send_list_row_parent_class)->dispose(object);
}

static void
ftc_home_send_list_row_finalize(GObject *object)
{
    G_OBJECT_CLASS(ftc_home_send_list_row_parent_class)->finalize(object);
}

static void
ftc_home_send_list_row_class_init (FtcHomeSendListRowClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_home_send_list_row_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_home_send_list_row_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-home-send-list-row.ui");
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListRow, img_file_icon);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListRow, lbl_file_name);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListRow, lbl_file_size);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListRow, lbl_progress);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListRow, pbar_progress);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcHomeSendListRow, box_progress_area);
}

FtcHomeSendListRow *
ftc_home_send_list_row_new (Ftc::Core::FileInfo &fi)
{
    FtcHomeSendListRow *row = FTC_HOME_SEND_LIST_ROW(g_object_new (FTC_HOME_SEND_LIST_ROW_TYPE, NULL));
    FtcHomeSendListRowPrivate *priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    std::string pixel_currect, show_filename;
    GdkPixbuf *pixbuf = NULL;
    
    priv->file_name = g_strdup(fi.getFileName().c_str());
    priv->file_path = g_strdup(fi.getFullPath().c_str());
    priv->relative_dir = g_strdup(fi.getRelativeDir().c_str());
    priv->file_size = fi.length();

    //  상대경로가 있으면 폴더 업로드이므로 상대경로를 출력해준다.
    if (fi.getRelativeDir().length() > 0) {
        show_filename = fi.getRelativeDir() + "/" + fi.getFileName();
    } else {
        show_filename = fi.getFileName();
    }

    //  pixel 계산
    pixel_currect = ftc_ui_reduce_text_pixel(GTK_WIDGET(priv->lbl_file_name), show_filename.c_str(), 170);
    gtk_label_set_text(priv->lbl_file_name, pixel_currect.c_str());
    if (pixel_currect.compare(show_filename) != 0) {
        gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_file_name), show_filename.c_str());
    }

    gtk_label_set_text(priv->lbl_file_size, ftc_ui_get_file_size_format(priv->file_size).c_str());

    //  icon
    pixbuf = ftc_ui_get_icon_pixbuf(priv->file_name, 24);
    gtk_image_set_from_pixbuf(priv->img_file_icon, pixbuf);

    if (pixbuf) {
        g_object_unref(pixbuf);
    }

    return row;
}

const char*             
ftc_home_send_list_row_get_file_name(FtcHomeSendListRow *row)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    const char *rv = NULL;
    
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = priv->file_name;
    return rv;
}

const char*             
ftc_home_send_list_row_get_file_path(FtcHomeSendListRow *row)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    const char *rv = NULL;
    
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = priv->file_path;

    return rv;
}

const char*             
ftc_home_send_list_row_get_relative_dir(FtcHomeSendListRow *row)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    const char *rv = NULL;
    
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = priv->relative_dir;
    return rv;
}

long long               
ftc_home_send_list_row_get_file_size(FtcHomeSendListRow *row)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    long long rv = 0;
    
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = priv->file_size;

    return rv;
}

void                    
ftc_home_send_list_row_set_upload_status(FtcHomeSendListRow *row, Ftc::Core::Transfer::UploadStatus status)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    priv->status = status;
}

Ftc::Core::Transfer::UploadStatus 
ftc_home_send_list_row_get_upload_status(FtcHomeSendListRow *row)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    Ftc::Core::Transfer::UploadStatus rv = Ftc::Core::Transfer::UploadStatus::UPLOAD_NONE;
    
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = priv->status;

    return rv;
}

void                    
ftc_home_send_list_row_set_progress(FtcHomeSendListRow *row, int progress)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    double value = 0, cur_value = 0;
    std::string str;
    
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }
    
    if (progress > 100) {
        progress = 100;
    } else if (progress < 0) {
        progress = 0;
    }

    if (progress > 0) {
        value = (double)progress / 100;
    }

    cur_value = gtk_progress_bar_get_fraction(priv->pbar_progress);

    if (cur_value != value) {
        gtk_progress_bar_set_fraction(priv->pbar_progress, value);
        str = std::to_string(progress) + "%";
        gtk_label_set_text(priv->lbl_progress, str.c_str());
    }

    if (progress == 100) {
        priv->status = Ftc::Core::Transfer::UploadStatus::UPLOADED;
    } else {
        priv->status = Ftc::Core::Transfer::UploadStatus::UPLOADING;
    }
}

void                    
ftc_home_send_list_row_set_visible_progressbar(FtcHomeSendListRow *row, bool is_visible)
{
    FtcHomeSendListRowPrivate *priv = NULL;
    
    if (! FTC_IS_HOME_SEND_LIST_ROW(row)) {
        return;
    }

    priv = (FtcHomeSendListRowPrivate*)ftc_home_send_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    gtk_widget_set_visible(GTK_WIDGET(priv->box_progress_area), is_visible);
}
