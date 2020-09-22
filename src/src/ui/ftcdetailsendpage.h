#ifndef _FTC_UI_DETAIL_SEND_PAGE_H_
#define _FTC_UI_DETAIL_SEND_PAGE_H_
#include "ftcuicommon.h"
#include "ftchomesendlistsubpage.h"
#include "../core/model/UploadOption.h"
#include "../core/model/PackFilePop.h"



#define FTC_DETAIL_SEND_PAGE_TYPE (ftc_detail_send_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcDetailSendPage, ftc_detail_send_page, FTC, DETAIL_SEND_PAGE, GtkBox)


FtcDetailSendPage*      ftc_detail_send_page_new(FtcHomeSendListSubPage *sendlist_sub_page);

void                    ftc_detail_send_page_disable(FtcDetailSendPage *page);

void                    ftc_detail_send_page_enable(FtcDetailSendPage *page, Ftc::Core::PackFilePop *pack_file_pop);

Ftc::Core::UploadOption*           ftc_detail_send_page_get_upload_option(FtcDetailSendPage *page);

bool                    ftc_detail_send_page_is_approval_update(FtcDetailSendPage *page);

#endif /* _FTC_UI_DETAIL_SEND_PAGE_H_ */
