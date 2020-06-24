#ifndef _FTC_UI_MAIN_WINDOW_H_
#define _FTC_UI_MAIN_WINDOW_H_
#include "ftcuicommon.h"
#include "ftcapplication.h"

#define FTC_TYPE_MAIN_WINDOW (ftc_main_window_get_type ())
G_DECLARE_FINAL_TYPE (FtcMainWindow, ftc_main_window, FTC, MAIN_WINDOW, GtkApplicationWindow)



FtcMainWindow       *ftc_main_window_new          (FtcApplication *app);
void                 ftc_main_window_open         (FtcMainWindow *main_window,
                                                         GFile            *file);

void                ftc_main_window_session_logout(FtcMainWindow *main_window, bool is_expired);

void                ftc_main_window_set_detail_send_page(FtcMainWindow *main_window, gpointer page);
void                ftc_main_window_set_detail_recv_page(FtcMainWindow *main_window, gpointer page);
void                ftc_main_window_set_detail_send_history_page(FtcMainWindow *main_window, gpointer page);
gpointer            ftc_main_window_get_detail_send_page(FtcMainWindow *main_window);
gpointer            ftc_main_window_get_detail_recv_page(FtcMainWindow *main_window);
gpointer            ftc_main_window_get_detail_send_history_page(FtcMainWindow *main_window);




#endif /* _FTC_UI_MAIN_WINDOW_H_ */
