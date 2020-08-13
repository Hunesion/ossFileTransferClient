#include "ftcdetailrecvfilelistrow.h"

struct _FtcDetailRecvFileListRow
{
    GtkBox parent;
};

typedef struct _FtcDetailRecvFileListRowPrivate FtcDetailRecvFileListRowPrivate;

struct _FtcDetailRecvFileListRowPrivate
{
    GtkCheckButton      *ckbtn_select;
    GtkImage            *img_icon;
    GtkLabel            *lbl_file_name;
    GtkLabel            *lbl_file_size;
    GtkLabel            *lbl_download_count;
    GtkBox              *box_progress_area;
    GtkLabel            *lbl_progress;
    GtkProgressBar      *pbar_progress;

    char                *file_uid;
    bool                is_expired;
};

G_DEFINE_TYPE_WITH_PRIVATE(FtcDetailRecvFileListRow, ftc_detail_recv_file_list_row, GTK_TYPE_BOX)

////////////////////////    Private 선언   ////////////////////////

///////////////////////////////////////////////////////////////////

static void
ftc_detail_recv_file_list_row_init (FtcDetailRecvFileListRow *row)
{
    FtcDetailRecvFileListRowPrivate *priv = NULL;
    gtk_widget_init_template (GTK_WIDGET (row));
    priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private (row);

    //  Todo..
    //  Signal (Event) 함수 연결

    //  Widget CSS 적용
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_file_name), "label_recv_info_title");
    gtk_widget_set_name(GTK_WIDGET(priv->lbl_progress), "label_send_progress");
    gtk_widget_set_name(GTK_WIDGET(priv->pbar_progress), "progressbar_individual_upload");
    if (ftc_ui_is_internal_location()) {
        //  업무 망 
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_file_size), "label_recv_info_contents_is");
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_download_count), "label_recv_info_contents_is");
    } else {
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_file_size), "label_recv_info_contents_es");    
        gtk_widget_set_name(GTK_WIDGET(priv->lbl_download_count), "label_recv_info_contents_es");
    }

}

static void
ftc_detail_recv_file_list_row_dispose (GObject *object)
{
    FtcDetailRecvFileListRowPrivate *priv;
    priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private(FTC_DETAIL_RECV_FILE_LIST_ROW(object));

    if (priv->file_uid) {
        g_free(priv->file_uid);
        priv->file_uid = NULL;
    }

    Ftc::Core::EventManager::getInstance()->removeEventListener(object);

    G_OBJECT_CLASS(ftc_detail_recv_file_list_row_parent_class)->dispose(object);
}

static void
ftc_detail_recv_file_list_row_finalize(GObject *object)
{

    G_OBJECT_CLASS(ftc_detail_recv_file_list_row_parent_class)->finalize(object);
}

static void
ftc_detail_recv_file_list_row_class_init (FtcDetailRecvFileListRowClass *clz)
{
    G_OBJECT_CLASS (clz)->finalize = ftc_detail_recv_file_list_row_finalize;
    G_OBJECT_CLASS (clz)->dispose = ftc_detail_recv_file_list_row_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(clz), "/com/hunesion/ftcapp/ftc-detail-recv-file-list-row.ui");
    
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvFileListRow, ckbtn_select);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvFileListRow, img_icon);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvFileListRow, lbl_file_name);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvFileListRow, lbl_file_size);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvFileListRow, lbl_download_count);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvFileListRow, box_progress_area);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvFileListRow, lbl_progress);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(clz), FtcDetailRecvFileListRow, pbar_progress);
}

FtcDetailRecvFileListRow *
ftc_detail_recv_file_list_row_new ()
{
    FtcDetailRecvFileListRow *row = FTC_DETAIL_RECV_FILE_LIST_ROW(g_object_new (FTC_DETAIL_RECV_FILE_LIST_ROW_TYPE, NULL));
    FtcDetailRecvFileListRowPrivate *priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private(row);
    return row;
}

const char*                 
ftc_detail_recv_file_list_row_get_file_uid(FtcDetailRecvFileListRow *row)
{
    FtcDetailRecvFileListRowPrivate *priv = NULL;
    const char *rv = NULL;
    if (! FTC_IS_DETAIL_RECV_FILE_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = priv->file_uid;
    return rv;
}

bool                        
ftc_detail_recv_file_list_row_get_check_active(FtcDetailRecvFileListRow *row)
{
    FtcDetailRecvFileListRowPrivate *priv = NULL;
    bool rv = false;
    if (! FTC_IS_DETAIL_RECV_FILE_LIST_ROW(row)) {
        return rv;
    }

    priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private(row);
    if (! priv) {
        return rv;
    }

    rv = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->ckbtn_select));
    return rv;
}

void                        
ftc_detail_recv_file_list_row_set_check_active(FtcDetailRecvFileListRow *row, bool check)
{
    FtcDetailRecvFileListRowPrivate *priv = NULL;

    if (! FTC_IS_DETAIL_RECV_FILE_LIST_ROW(row)) {
        return;
    }

    priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    if (priv->is_expired == false) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->ckbtn_select), check);
    }
}

void 
ftc_detail_recv_file_list_row_set_data(FtcDetailRecvFileListRow *row, Ftc::Core::GlobalStruct::FtcRequestInfoFile &file_info, bool is_expired_date)
{
    FtcDetailRecvFileListRowPrivate *priv = NULL;
    std::time_t now_date = std::time(NULL), tmp_date, calc_date;
    std::string str, show_filename;
    GdkPixbuf *pixbuf = NULL;

    if (! FTC_IS_DETAIL_RECV_FILE_LIST_ROW(row)) {
        return;
    }
    priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    if (priv->file_uid) {
        g_free(priv->file_uid);
    }
    priv->file_uid = g_strdup(file_info.request_info_file_list_uid.c_str()); 

    //  파일이름
    if (file_info.relative_dir.length() > 0) {
        show_filename = file_info.relative_dir + "/" + file_info.file_name;
    } else {
        show_filename = file_info.file_name;
    }
    str = ftc_ui_reduce_text_pixel(GTK_WIDGET(priv->lbl_file_name), show_filename.c_str(), 175);
    gtk_label_set_text(priv->lbl_file_name, str.c_str());
    //  튤팁
    if (str.compare(show_filename) != 0) {
        gtk_widget_set_tooltip_text(GTK_WIDGET(priv->lbl_file_name), show_filename.c_str());
    }

    //  파일사이즈
    gtk_label_set_text(priv->lbl_file_size, ftc_ui_get_file_size_format(file_info.file_size).c_str());

    //  다운로드 횟수
    str = std::to_string(file_info.download_cnt) + "/" + std::to_string(file_info.download_limit_cnt);
    gtk_label_set_text(priv->lbl_download_count, str.c_str());

    //  파일 유효기간 및 다운로드 횟수 확인하여 체크박스 비활성화
    if (is_expired_date || (file_info.download_limit_cnt <= file_info.download_cnt)) {
        gtk_widget_set_visible(GTK_WIDGET(priv->ckbtn_select), FALSE);
        gtk_widget_set_margin_start(GTK_WIDGET(priv->img_icon), 38);

        priv->is_expired = true;
    }

    //  icon
    pixbuf = ftc_ui_get_icon_pixbuf(file_info.file_name.c_str(), 24);
    gtk_image_set_from_pixbuf(priv->img_icon, pixbuf);

    if (pixbuf) {
        g_object_unref(pixbuf);
    }
}

void                        
ftc_detail_recv_file_list_row_set_visible_progress(FtcDetailRecvFileListRow *row, bool is_visible)
{
    FtcDetailRecvFileListRowPrivate *priv = NULL;

    if (! FTC_IS_DETAIL_RECV_FILE_LIST_ROW(row)) {
        return;
    }

    priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private(row);
    if (! priv) {
        return;
    }

    if (is_visible) {
        gtk_widget_set_visible(GTK_WIDGET(priv->box_progress_area), true);

        gtk_widget_set_visible(GTK_WIDGET(priv->lbl_file_size), false);
        gtk_widget_set_visible(GTK_WIDGET(priv->lbl_download_count), false);
    } else {
        gtk_widget_set_visible(GTK_WIDGET(priv->box_progress_area), false);

        gtk_widget_set_visible(GTK_WIDGET(priv->lbl_file_size), true);
        gtk_widget_set_visible(GTK_WIDGET(priv->lbl_download_count), true);
    }

}

void
ftc_detail_recv_file_list_row_set_progress(FtcDetailRecvFileListRow *row, int progress)
{
    FtcDetailRecvFileListRowPrivate *priv = NULL;
    std::string str;
    double value = 0, cur_value = 0;

    if (! FTC_IS_DETAIL_RECV_FILE_LIST_ROW(row)) {
        return;
    }

    priv = (FtcDetailRecvFileListRowPrivate*)ftc_detail_recv_file_list_row_get_instance_private(row);
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
}