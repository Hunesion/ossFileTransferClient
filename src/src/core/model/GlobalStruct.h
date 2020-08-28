#ifndef _FTC_CORE_MODEL_GLOBAL_STRUCT_H_
#define _FTC_CORE_MODEL_GLOBAL_STRUCT_H_

#include <string>
#include <vector>
#include <list>
#include "../macro.h"

BEGIN_FTC_CORE
namespace GlobalStruct
{
    typedef struct _FtcRequestApprovalInfo
    {
        std::string approval_state;
        std::string approval_type;
        std::string last_approval_type;
    } FtcRequestApprovalInfo;

    typedef struct _FtcRequestInfoFile{
        std::string         request_info_file_list_uid;
        std::string         file_name;
        std::string         file_path;
        std::string         relative_dir;
        std::string         server_file_name;
        std::string         md5sum;
        int                 file_size;
        int                 download_limit_cnt;
        int                 download_cnt;
        
        _FtcRequestInfoFile()
        {
            file_size = 0;
        }
    } FtcRequestInfoFile;

    typedef struct _FtcRequestInfo{
        std::string     request_user_name;
        std::string     request_user_id;
        std::string     request_user_group_name;
        std::string     request_text;
        std::string     receive_name;

        std::string     receive_id;
        std::string     receive_user_group_name;
    } FtcRequestInfo;

    typedef struct _FtcNetwork
    {
        std::string direction;
        std::string from_network_id;
        std::string from_network_name;
        std::string network_direction_id;
        std::string to_name;
        std::string to_network_name;
        std::string to_network_id;
    } FtcNetwork;

    typedef struct _FtcRequest {
        std::vector<FtcRequestInfoFile> vc_file_info;
        FtcRequestInfo      ftc_request_info;
        FtcRequestApprovalInfo ftc_request_approval_info;
        std::string         request_info_uid;
        std::string         request_title;
        std::string         request_user_name;
        std::string         request_user_id;
        std::string         request_dt;
        std::string         firstFileName;
        std::string         from_network_id;
        std::string         from_network_name;
        std::string         to_network_id;
        std::string         to_network_name;
        std::string         download_expire_dt;
        int                 file_cnt;
        int                 totalFileSize;
        bool                is_read;

        _FtcRequest()
        {
            file_cnt = 0;
            totalFileSize = 0;
            is_read = false;
        }

        
    } FtcRequest;

    typedef struct _FtcTargetInfo
    {
        std::string userid;
        std::string name;
        std::string dept;
        std::string network_name;
        std::string network_type;
        std::string network_location;
        std::string network_id;
        std::string ip;
    } FtcTargetInfo;
}

END_FTC_CORE


#endif /* _FTC_CORE_MODEL_GLOBAL_STRUCT_H_ */