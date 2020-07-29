#ifndef _FTC_UI_HOME_HEADER_SEND_BOX_H_
#define _FTC_UI_HOME_HEADER_SEND_BOX_H_
#include "ftcuicommon.h"
#include "ftchomepage.h"


#define FTC_HOME_HEADER_SEND_BOX_TYPE (ftc_home_header_send_box_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeHeaderSendBox, ftc_home_header_send_box, FTC, HOME_HEADER_SEND_BOX, GtkBox)


FtcHomeHeaderSendBox*    ftc_home_header_send_box_new(FtcHomePage *home_page);




#endif /* _FTC_UI_HOME_HEADER_SEND_BOX_H_ */
