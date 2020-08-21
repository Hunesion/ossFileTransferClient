#ifndef _FTC_UI_HOME_SEND_HISTORY_ROW_H_
#define _FTC_UI_HOME_SEND_HISTORY_ROW_H_
#include "ftcuicommon.h"
#include "ftchomesendhistorysubpage.h"

#define FTC_HOME_SEND_HISTORY_ROW_TYPE (ftc_home_send_history_row_get_type ())
G_DECLARE_FINAL_TYPE (FtcHomeSendHistoryRow, ftc_home_send_history_row, FTC, HOME_SEND_HISTORY_ROW, GtkBox)


FtcHomeSendHistoryRow*     ftc_home_send_history_row_new(FtcHomeSendHistorySubPage *sendhistory_sub_page, Ftc::Core::GlobalStruct::FtcRequest *ftc_request);

Ftc::Core::GlobalStruct::FtcRequest* ftc_home_send_history_row_get_ftc_request(FtcHomeSendHistoryRow *row);

RequestDownloadState*  ftc_home_send_history_row_get_request_download_state(FtcHomeSendHistoryRow *row);

void                    ftc_home_send_history_row_update_title_read(FtcHomeSendHistoryRow *row);

void                    ftc_home_send_history_row_set_visible_progress(FtcHomeSendHistoryRow *row, bool is_visible);

void                    ftc_home_send_history_row_set_count_progress(FtcHomeSendHistoryRow *row, int count, int total_count);

void                    ftc_home_send_history_row_add_count_progress(FtcHomeSendHistoryRow *row);

void                    ftc_home_send_history_row_set_progress_background_value(FtcHomeSendHistoryRow *row, const char *request_info_file_list_uid, int progress);

void                    ftc_home_send_history_row_download_complete(FtcHomeSendHistoryRow *row);

#endif /* _FTC_UI_HOME_SEND_HISTORY_ROW_H_ */
