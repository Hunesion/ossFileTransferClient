#ifndef _FTC_UI_HOME_RECV_LIST_ROW_H_
#define _FTC_UI_HOME_RECV_LIST_ROW_H_
#include "ftcuicommon.h"
#include "ftchomerecvlistsubpage.h"

#define FTC_HOME_RECV_LIST_ROW_TYPE (ftc_home_recv_list_row_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeRecvListRow, ftc_home_recv_list_row, FTC, HOME_RECV_LIST_ROW, GtkBox)


FtcHomeRecvListRow*     ftc_home_recv_list_row_new(FtcHomeRecvListSubPage *recvlist_sub_page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request);

Ftc::Core::GlobalStruct::FtcRequest* ftc_home_recv_list_row_get_ftc_request(FtcHomeRecvListRow *row);

RequestDownloadState*  ftc_home_recv_list_row_get_request_download_state(FtcHomeRecvListRow *row);

void                    ftc_home_recv_list_row_update_title_read(FtcHomeRecvListRow *row);

void                    ftc_home_recv_list_row_set_visible_progress(FtcHomeRecvListRow *row, bool is_visible);

void                    ftc_home_recv_list_row_set_count_progress(FtcHomeRecvListRow *row, int count, int total_count);

void                    ftc_home_recv_list_row_add_count_progress(FtcHomeRecvListRow *row);

void                    ftc_home_recv_list_row_set_progress_background_value(FtcHomeRecvListRow *row, const char *request_info_file_list_uid, int progress);

void                    ftc_home_recv_list_row_download_complete(FtcHomeRecvListRow *row);

#endif /* _FTC_UI_HOME_RECV_LIST_ROW_H_ */
