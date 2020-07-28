#ifndef _FTC_UI_LOGIN_PAGE_H_
#define _FTC_UI_LOGIN_PAGE_H_
#include "ftcuicommon.h"
#include "ftcmainwindow.h"


#define FTC_LOGIN_PAGE_TYPE (ftc_login_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcLoginPage, ftc_login_page, FTC, LOGIN_PAGE, GtkBox)


FtcLoginPage    *ftc_login_page_new();

void ftc_login_page_do_login(FtcLoginPage *page);

void ftc_login_page_set_login_state(FtcLoginPage *page, bool state);

bool ftc_login_page_get_login_state(FtcLoginPage *page);


#endif /* _FTC_UI_LOGIN_PAGE_H_ */
