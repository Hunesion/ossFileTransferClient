#ifndef _FTC_UI_HOME_PAGE_H_
#define _FTC_UI_HOME_PAGE_H_
#include "ftcuicommon.h"





#define FTC_HOME_PAGE_TYPE (ftc_home_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomePage, ftc_home_page, FTC, HOME_PAGE, GtkBox)


FtcHomePage*    ftc_home_page_new();

const char*     ftc_home_page_get_current_page(FtcHomePage *page);

void            ftc_home_page_recv_list_refresh(FtcHomePage *page);


#endif /* _FTC_UI_HOME_PAGE_H_ */
