#ifndef _FTC_UI_DETAIL_SEND_HISTORY_FILE_LIST_ROW_H
#define _FTC_UI_DETAIL_SEND_HISTORY_FILE_LIST_ROW_H
#include "ftcuicommon.h"


#define FTC_DETAIL_SEND_HISTORY_FILE_LIST_ROW_TYPE (ftc_detail_send_history_file_list_row_get_type ())
G_DECLARE_FINAL_TYPE (FtcDetailSendHistoryFileListRow, ftc_detail_send_history_file_list_row, FTC, DETAIL_SEND_HISTORY_FILE_LIST_ROW, GtkBox)


FtcDetailSendHistoryFileListRow*   ftc_detail_send_history_file_list_row_new();

void                        ftc_detail_send_history_file_list_row_set_data(FtcDetailSendHistoryFileListRow *row, Ftc::Core::GlobalStruct::FtcRequestInfoFile &file_info, bool is_expired_date);

const char*                 ftc_detail_send_history_file_list_row_get_file_uid(FtcDetailSendHistoryFileListRow *row);

bool                        ftc_detail_send_history_file_list_row_get_check_active(FtcDetailSendHistoryFileListRow *row);

void                        ftc_detail_send_history_file_list_row_set_check_active(FtcDetailSendHistoryFileListRow *row, bool check);

void                        ftc_detail_send_history_file_list_row_set_visible_progress(FtcDetailSendHistoryFileListRow *row, bool is_visible);

void                        ftc_detail_send_history_file_list_row_set_progress(FtcDetailSendHistoryFileListRow *row, int progress);

#endif /* _FTC_UI_DETAIL_SEND_HISTORY_FILE_LIST_ROW_H */
