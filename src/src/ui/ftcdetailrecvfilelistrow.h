#ifndef _FTC_UI_DETAIL_RECV_FILE_LIST_ROW_H
#define _FTC_UI_DETAIL_RECV_FILE_LIST_ROW_H
#include "ftcuicommon.h"


#define FTC_DETAIL_RECV_FILE_LIST_ROW_TYPE (ftc_detail_recv_file_list_row_get_type ())
G_DECLARE_FINAL_TYPE (FtcDetailRecvFileListRow, ftc_detail_recv_file_list_row, FTC, DETAIL_RECV_FILE_LIST_ROW, GtkBox)


FtcDetailRecvFileListRow*   ftc_detail_recv_file_list_row_new();

void                        ftc_detail_recv_file_list_row_set_data(FtcDetailRecvFileListRow *row, Ftc::Core::GlobalStruct::FtcRequestInfoFile &file_info, bool is_expired_date);

const char*                 ftc_detail_recv_file_list_row_get_file_uid(FtcDetailRecvFileListRow *row);

bool                        ftc_detail_recv_file_list_row_get_check_active(FtcDetailRecvFileListRow *row);

void                        ftc_detail_recv_file_list_row_set_check_active(FtcDetailRecvFileListRow *row, bool check);

void                        ftc_detail_recv_file_list_row_set_visible_progress(FtcDetailRecvFileListRow *row, bool is_visible);

void                        ftc_detail_recv_file_list_row_set_progress(FtcDetailRecvFileListRow *row, int progress);

#endif /* _FTC_UI_DETAIL_RECV_FILE_LIST_ROW_H */
