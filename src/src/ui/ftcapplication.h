#ifndef _FTC_UI_APPLICATION_H_
#define _FTC_UI_APPLICATION_H_
#include "ftcuicommon.h"

#define FTC_TYPE_APPLICATION (ftc_application_get_type ())
G_DECLARE_FINAL_TYPE (FtcApplication, ftc_application, FTC, APPLICATION, GtkApplication)


FtcApplication     *ftc_application_new         (void);


#endif /* _FTC_UI_APPLICATION_H_ */
