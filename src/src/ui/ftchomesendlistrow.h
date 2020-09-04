#ifndef _FTC_UI_HOME_SEND_LIST_ROW_H_
#define _FTC_UI_HOME_SEND_LIST_ROW_H_
#include "ftcuicommon.h"


#define FTC_HOME_SEND_LIST_ROW_TYPE (ftc_home_send_list_row_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeSendListRow, ftc_home_send_list_row, FTC, HOME_SEND_LIST_ROW, GtkBox)


FtcHomeSendListRow*     ftc_home_send_list_row_new(Ftc::Core::FileInfo &fi);

const char*             ftc_home_send_list_row_get_file_name(FtcHomeSendListRow *row);

const char*             ftc_home_send_list_row_get_file_path(FtcHomeSendListRow *row);

const char*             ftc_home_send_list_row_get_relative_dir(FtcHomeSendListRow *row);

long long               ftc_home_send_list_row_get_file_size(FtcHomeSendListRow *row);

void                    ftc_home_send_list_row_set_upload_status(FtcHomeSendListRow *row, Ftc::Core::Transfer::UploadStatus status);

Ftc::Core::Transfer::UploadStatus ftc_home_send_list_row_get_upload_status(FtcHomeSendListRow *row);

void                    ftc_home_send_list_row_set_progress(FtcHomeSendListRow *row, int progress);

void                    ftc_home_send_list_row_set_visible_progressbar(FtcHomeSendListRow *row, bool is_visible);

#endif /* _FTC_UI_HOME_SEND_LIST_ROW_H_ */
