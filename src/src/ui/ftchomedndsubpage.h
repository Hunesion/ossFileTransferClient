#ifndef _FTC_UI_HOME_DND_SUB_PAGE_H_
#define _FTC_UI_HOME_DND_SUB_PAGE_H_
#include "ftcuicommon.h"
#include "ftchomepage.h"


#define FTC_HOME_DND_SUB_PAGE_TYPE (ftc_home_dnd_sub_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeDndSubPage, ftc_home_dnd_sub_page, FTC, HOME_DND_SUB_PAGE, GtkBox)


FtcHomeDndSubPage   *ftc_home_dnd_sub_page_new(FtcHomePage *home_page);

void                ftc_home_dnd_sub_page_update_last_request(FtcHomeDndSubPage *page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request);

void                ftc_home_dnd_sub_page_set_new_recv_request_count(FtcHomeDndSubPage *page, int new_count);

#endif /* _FTC_UI_HOME_DND_SUB_PAGE_H_ */
