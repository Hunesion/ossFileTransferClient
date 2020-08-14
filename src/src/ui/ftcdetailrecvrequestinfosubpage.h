#ifndef _FTC_UI_DETAIL_RECV_REQUEST_INFO_SUB_PAGE_H_
#define _FTC_UI_DETAIL_RECV_REQUEST_INFO_SUB_PAGE_H_
#include "ftcuicommon.h"




#define FTC_DETAIL_RECV_REQUEST_INFO_SUB_PAGE_TYPE (ftc_detail_recv_request_info_sub_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcDetailRecvRequestInfoSubPage, ftc_detail_recv_request_info_sub_page, FTC, DETAIL_RECV_REQUEST_INFO_SUB_PAGE, GtkBox)


FtcDetailRecvRequestInfoSubPage*    ftc_detail_recv_request_info_sub_page_new();

void                                ftc_detail_recv_request_info_sub_page_set_data(FtcDetailRecvRequestInfoSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request);

#endif /* _FTC_UI_DETAIL_RECV_REQUEST_INFO_SUB_PAGE_H_ */
