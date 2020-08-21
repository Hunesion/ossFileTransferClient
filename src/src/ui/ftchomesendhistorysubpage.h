#ifndef _FTC_UI_HOME_SEND_HISTORY_SUB_PAGE_H_
#define _FTC_UI_HOME_SEND_HISTORY_SUB_PAGE_H_
#include "ftcuicommon.h"
#include "ftchomepage.h"

#define FTC_HOME_SEND_HISTORY_SUB_PAGE_TYPE (ftc_home_send_history_sub_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeSendHistorySubPage, ftc_home_send_history_sub_page, FTC, HOME_SEND_HISTORY_SUB_PAGE, GtkBox)



FtcHomeSendHistorySubPage*      ftc_home_send_history_sub_page_new(FtcHomePage *home_page);

void                            ftc_home_send_history_sub_page_get_send_history_list(FtcHomeSendHistorySubPage *page);

void                            ftc_home_send_history_sub_page_list_box_select_row_and_batch_download(FtcHomeSendHistorySubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request);

void                            ftc_home_send_history_sub_page_list_box_unselect_all(FtcHomeSendHistorySubPage *page);

void                            ftc_home_send_history_sub_page_set_visible_progress(FtcHomeSendHistorySubPage *page, const char *request_info_uid, bool is_visible, int total_count);

void                            ftc_home_send_history_sub_page_set_progress_background_value(FtcHomeSendHistorySubPage *page, const char *request_info_uid, const char *request_info_file_list_uid, int progress);

void                            ftc_home_send_history_sub_page_update_total_progress(FtcHomeSendHistorySubPage *page, const char *request_info_uid);

void                            ftc_home_send_history_sub_page_download_complete(FtcHomeSendHistorySubPage *page, const char *request_info_uid);


#endif /* _FTC_UI_HOME_SEND_HISTORY_SUB_PAGE_H_ */
