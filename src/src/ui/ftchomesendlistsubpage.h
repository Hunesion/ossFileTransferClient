#ifndef _FTC_UI_HOME_SEND_LIST_SUB_PAGE_H_
#define _FTC_UI_HOME_SEND_LIST_SUB_PAGE_H_
#include "ftcuicommon.h"
#include "ftchomepage.h"




#define FTC_HOME_SEND_LIST_SUB_PAGE_TYPE (ftc_home_send_list_sub_page_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeSendListSubPage, ftc_home_send_list_sub_page, FTC, HOME_SEND_LIST_SUB_PAGE, GtkBox)


FtcHomeSendListSubPage*     ftc_home_send_list_sub_page_new(FtcHomePage *home_page);

bool                        ftc_home_send_list_sub_page_enable(FtcHomeSendListSubPage *page);

void                        ftc_home_send_list_sub_page_disable(FtcHomeSendListSubPage *page);

void                        ftc_home_send_list_sub_page_add_list(FtcHomeSendListSubPage *page, const char *path);

void                        ftc_home_send_list_sub_page_add_list_file_info(FtcHomeSendListSubPage *page, Ftc::Core::FileInfo &fi);

int                         ftc_home_send_list_sub_page_list_box_get_size(FtcHomeSendListSubPage *page);

void                        ftc_home_send_list_sub_page_activate_extension_window(FtcHomeSendListSubPage *page, bool only_extension);

bool                        ftc_home_send_list_sub_page_is_uploading(FtcHomeSendListSubPage *page);




#endif /* _FTC_UI_HOME_SEND_LIST_SUB_PAGE_H_ */
