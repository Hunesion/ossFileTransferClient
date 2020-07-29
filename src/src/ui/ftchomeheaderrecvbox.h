#ifndef _FTC_UI_HOME_HEADER_RECV_BOX_H_
#define _FTC_UI_HOME_HEADER_RECV_BOX_H_
#include "ftcuicommon.h"
#include "ftchomepage.h"


#define FTC_HOME_HEADER_RECV_BOX_TYPE (ftc_home_header_recv_box_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeHeaderRecvBox, ftc_home_header_recv_box, FTC, HOME_HEADER_RECV_BOX, GtkBox)


FtcHomeHeaderRecvBox*   ftc_home_header_recv_box_new(FtcHomePage *home_page, const char *next_page_name);

void                    ftc_home_header_recv_box_set_new_recv_request_count(FtcHomeHeaderRecvBox *box, int count);

#endif /* _FTC_UI_HOME_HEADER_RECV_BOX_H_ */
