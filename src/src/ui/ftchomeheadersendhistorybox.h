#ifndef _FTC_UI_HOME_HEADER_SEND_HISTORY_BOX_H_
#define _FTC_UI_HOME_HEADER_SEND_HISTORY_BOX_H_
#include "ftcuicommon.h"
#include "ftchomepage.h"


#define FTC_HOME_HEADER_SEND_HISTORY_BOX_TYPE (ftc_home_header_send_history_box_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeHeaderSendHistoryBox, ftc_home_header_send_history_box, FTC, HOME_HEADER_SEND_HISTORY_BOX, GtkBox)


FtcHomeHeaderSendHistoryBox*   ftc_home_header_send_history_box_new(FtcHomePage *home_page);

void                            ftc_home_header_send_history_set_new_count(FtcHomeHeaderSendHistoryBox *box, int new_count);

#endif /* _FTC_UI_HOME_HEADER_SEND_HISTORY_BOX_H_ */
