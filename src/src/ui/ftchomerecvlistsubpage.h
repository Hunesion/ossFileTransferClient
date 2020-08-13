#ifndef _FTC_UI_HOME_RECV_LIST_SUB_PAGE_H_
#define _FTC_UI_HOME_RECV_LIST_SUB_PAGE_H_
#include "ftcuicommon.h"
#include "ftchomepage.h"

#define FTC_HOME_RECV_LIST_SUB_PAGE_TYPE (ftc_home_recv_list_sub_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeRecvListSubPage, ftc_home_recv_list_sub_page, FTC, HOME_RECV_LIST_SUB_PAGE, GtkBox)



FtcHomeRecvListSubPage*     ftc_home_recv_list_sub_page_new(FtcHomePage *home_page);

void                        ftc_home_recv_list_sub_page_get_receive_list(FtcHomeRecvListSubPage *page);

int                         ftc_home_recv_list_sub_page_get_recv_request_new_count(FtcHomeRecvListSubPage *page);

void                        ftc_home_recv_list_sub_page_list_box_select_row_and_batch_download(FtcHomeRecvListSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request);

void                        ftc_home_recv_list_sub_page_list_box_unselect_all(FtcHomeRecvListSubPage *page);

void                        ftc_home_recv_list_sub_page_set_visible_progress(FtcHomeRecvListSubPage *page, const char *request_info_uid, bool is_visible, int total_count);

void                        ftc_home_recv_list_sub_page_set_progress_background_value(FtcHomeRecvListSubPage *page, const char *request_info_uid, const char *request_info_file_list_uid, int progress);

void                        ftc_home_recv_list_sub_page_update_total_progress(FtcHomeRecvListSubPage *page, const char *request_info_uid);

void                        ftc_home_recv_list_sub_page_download_complete(FtcHomeRecvListSubPage *page, const char *request_info_uid);


#endif /* _FTC_UI_HOME_RECV_LIST_SUB_PAGE_H_ */
