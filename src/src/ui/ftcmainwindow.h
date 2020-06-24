#ifndef _FTC_UI_MAIN_WINDOW_H_
#define _FTC_UI_MAIN_WINDOW_H_
#include "ftcuicommon.h"
#include "ftcapplication.h"

#define FTC_TYPE_MAIN_WINDOW (ftc_main_window_get_type ())
G_DECLARE_FINAL_TYPE (FtcMainWindow, ftc_main_window, FTC, MAIN_WINDOW, GtkApplicationWindow)



FtcMainWindow       *ftc_main_window_new          (FtcApplication *app);
void                 ftc_main_window_open         (FtcMainWindow *main_window,
                                                         GFile            *file);



#endif /* _FTC_UI_MAIN_WINDOW_H_ */
