#ifndef _FTC_UI_DETAIL_RECV_PAGE_H_
#define _FTC_UI_DETAIL_RECV_PAGE_H_
#include "ftcuicommon.h"
#include "ftchomerecvlistsubpage.h"




#define FTC_DETAIL_RECV_PAGE_TYPE (ftc_detail_recv_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcDetailRecvPage, ftc_detail_recv_page, FTC, DETAIL_RECV_PAGE, GtkBox)


FtcDetailRecvPage*      ftc_detail_recv_page_new(FtcHomeRecvListSubPage *recvlist_sub_page);

void                    ftc_detail_recv_page_set_data(FtcDetailRecvPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request, RequestDownloadState *request_down_state);

bool                    ftc_detail_recv_page_file_download_all(FtcDetailRecvPage *page);


#endif /* _FTC_UI_DETAIL_RECV_PAGE_H_ */
