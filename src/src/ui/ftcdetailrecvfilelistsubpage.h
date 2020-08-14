#ifndef _FTC_UI_DETAIL_RECV_FILE_LIST_SUB_PAGE_H_
#define _FTC_UI_DETAIL_RECV_FILE_LIST_SUB_PAGE_H_
#include "ftcuicommon.h"




#define FTC_DETAIL_RECV_FILE_LIST_SUB_PAGE_TYPE (ftc_detail_recv_file_list_sub_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcDetailRecvFileListSubPage, ftc_detail_recv_file_list_sub_page, FTC, DETAIL_RECV_FILE_LIST_SUB_PAGE, GtkBox)


FtcDetailRecvFileListSubPage*      ftc_detail_recv_file_list_sub_page_new();

void                               ftc_detail_recv_file_list_sub_page_set_data(FtcDetailRecvFileListSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, RequestDownloadState *request_down_state);

void                               ftc_detail_recv_file_list_sub_page_set_all_check_active(FtcDetailRecvFileListSubPage *page, bool check);

GSList*                            ftc_detail_recv_file_list_sub_page_get_checked_file_uid(FtcDetailRecvFileListSubPage *page);

GSList*                            ftc_detail_recv_file_list_sub_page_get_all_file_uid(FtcDetailRecvFileListSubPage *page);

void                               ftc_detail_recv_file_list_sub_page_update_activation_view(FtcDetailRecvFileListSubPage *page);

void                               ftc_detail_recv_file_list_sub_page_set_progress_value(FtcDetailRecvFileListSubPage *page, const char *request_info_file_list_uid, int progress);

#endif /* _FTC_UI_DETAIL_RECV_FILE_LIST_SUB_PAGE_H_ */
