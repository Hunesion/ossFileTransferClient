#ifndef _FTC_UI_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE_H_
#define _FTC_UI_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE_H_
#include "ftcuicommon.h"
#include "ftcdetailsendhistorypage.h"

typedef struct _RequestInfo
{
    Ftc::Core::GlobalStruct::FtcNetwork network;
    Ftc::Core::GlobalStruct::FtcRequestApprovalInfo approval;
    std::string download_expire_dt;
    GtkWidget *widget_target_network;
    GtkWidget *event_box;
} RequestInfo;

#define FTC_DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE_TYPE (ftc_detail_send_history_request_info_sub_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcDetailSendHistoryRequestInfoSubPage, ftc_detail_send_history_request_info_sub_page, FTC, DETAIL_SEND_HISTORY_REQUEST_INFO_SUB_PAGE, GtkBox)


FtcDetailSendHistoryRequestInfoSubPage*    ftc_detail_send_history_request_info_sub_page_new(FtcDetailSendHistoryPage *page);

void                                ftc_detail_send_history_request_info_sub_page_set_data(FtcDetailSendHistoryRequestInfoSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, GSList *request_info_list);

#endif /* _FTC_UI_DETAIL_RECV_REQUEST_INFO_SUB_PAGE_H_ */
