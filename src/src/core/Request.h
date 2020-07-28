#ifndef _FTC_REQUEST_H_
#define _FTC_REQUEST_H_
#include <json-glib/json-glib.h>
#include <memory>
#include <list>
#include <map>
#include "macro.h"
#include "Response.h"

BEGIN_FTC_CORE

namespace Request{
    typedef struct _InsertPackFileNetworkParam
    {
        std::string receive_network;
        std::string download_expire_dt;
    } InsertPackFileNetworkParam;
    

    void init(); 
    bool requestLoginLocation(); 
    bool requestLogin(const char *userId , const char *passwd, bool auto_login, void *user_data = nullptr);
    bool requestLogout(bool isSessionTimeout = false);
    bool requestUsersCnt();
    bool requestSelectCsClipboardList();

    /**
     * File Upload Ready
     */
    std::shared_ptr<Response> requestInitPackFilePop();
    std::shared_ptr<Response> requestSelectEnableSendNetworkList();
    std::shared_ptr<Response> requestSelectNetworkSelectionUserList(const char *network_id, const char *user_id);
    std::shared_ptr<Response> requestGetUserExistCheckResult(const char *id_list);
    std::shared_ptr<Response> requestGetMaxDayUploadFileSizeResult(int send_file_sum, const char *network_id);
    std::shared_ptr<Response> requestGetUserCntResult(const char *receive_id);
    std::shared_ptr<Response> requestGetUserFileSumCheckResult(const char *receive_id, int send_file_sum);
    std::shared_ptr<Response> requestDeleteReceiveFileList();

    /**
     * File Upload
     */ 
    std::shared_ptr<Response> requestFileUploadFileName(const std::string &fileName); 
    std::shared_ptr<Response> requestFileUpload(const char *filename , const char *serverFileName, unsigned char **content , long long size , long long offset, long long totalSize, const char *sha); 
    std::shared_ptr<Response> requestFileUploadApi(const char *filename, unsigned char **content, long long size, const char *sys_name, const char *to_network_ip);

    /**
     * File Upload End
     */ 
    std::shared_ptr<Response> requestFileUploadCheckBefore(int count);
    bool requestFileUploadCheck(const char *receive_network, std::list<std::string> &uploadedFilesList, std::list<std::string> &requestInfoFileListUIDs, const char *event_name, gpointer user_data);
    std::shared_ptr<Response> requestFileUploadCheckAfter(const char *receive_network, std::list<std::string> &requestInfoFileListUIDs);
    std::shared_ptr<Response> requestInsertPackFile(const char *request_title, const char *request_text, bool is_approval_officer, const char *receive_id_list_hidden,
                                                    std::list<InsertPackFileNetworkParam> network_params, std::map<std::string, std::string> approval_lines, std::list<std::string> strFileList);
    
    /**
     * File Upload Delete
     */
    std::shared_ptr<Response> requestDeleteUploadGarbage(std::list<std::string> uploadedFilesList);
    std::shared_ptr<Response> requestInsertPackFileDelete(std::list<std::string> strFileList);

    /**
     * File Recv
     */
    std::shared_ptr<Response> requestSelectReceiveFileList_ajax(const unsigned int page, const char *from_date, const char* to_date);
    std::shared_ptr<Response> requestSelectPackFile_ajax(const char *request_info_uid, const char *list_type);
    bool requestLeftmenuCount_ajax(const char *list_type, const char *from_date, const char *to_date);

    /**
     * File History
     */ 
    std::shared_ptr<Response> requestSelectSendFileList_ajax(const unsigned int page, const char *from_date, const char* to_date);
    std::shared_ptr<Response> requestSelectPackFileNetwork_ajax(const char *request_info_uid, const char *from_network_id, const char *to_network_id, const char *list_type);

    /**
     * File Download
     */
    std::shared_ptr<Response> requestInitPackFileDownload_ajax(const char *request_info_uid, const char *ipaddress);
    bool requestFileDownload_isCsharp(const char *request_info_file_list_uid, const char *from_network_id, const char *to_network_id, const char *file_path);
    std::shared_ptr<Response> requestSelectNotDownloadFileList(const char *request_dt);

    /**
     * PCURL
     */
    std::shared_ptr<Response> requestSelectPcUrlProxy();
}
END_FTC_CORE
#endif /* _FTC_REQUEST_H_ */