#ifndef _FTC_UI_DETAIL_SEND_HISTORY_PAGE_H_
#define _FTC_UI_DETAIL_SEND_HISTORY_PAGE_H_
#include "ftcuicommon.h"
#include "ftchomesendhistorysubpage.h"


#define FTC_DETAIL_SEND_HISTORY_PAGE_TYPE (ftc_detail_send_history_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcDetailSendHistoryPage, ftc_detail_send_history_page, FTC, DETAIL_SEND_HISTORY_PAGE, GtkBox)


FtcDetailSendHistoryPage*      ftc_detail_send_history_page_new(FtcHomeSendHistorySubPage *sendhistory_sub_page);

void                    ftc_detail_send_history_page_set_data(FtcDetailSendHistoryPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, RequestDownloadState *request_down_state);

GSList*                 ftc_detail_send_history_page_update_approval_info(FtcDetailSendHistoryPage *page);

bool                    ftc_detail_send_history_page_file_download_all(FtcDetailSendHistoryPage *page);


#endif /* _FTC_UI_DETAIL_SEND_HISTORY_PAGE_H_ */
