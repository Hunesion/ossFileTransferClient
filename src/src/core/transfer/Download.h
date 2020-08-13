#ifndef _FTC_CORE_TRANSFER_DOWNLOAD_H_
#define _FTC_CORE_TRANSFER_DOWNLOAD_H_
#include <string>
#include <list>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "DownloadRequest.h"
#include "../macro.h"
#include "../EventManager.h"
#include "../JsonUtils.h"

BEGIN_FTC_CORE namespace Transfer{


enum DownloadStatus {
    DOWNLOAD_CANCEL = 0,
    DOWNLOADING,
    DOWNLOAD_COMPLETE,
    DOWNLOAD_NONE
};

typedef struct _DownloadResultParam {
    DownloadStatus status;
    JsonNode *node;
    gpointer user_data;
} DownloadResultParam;

class Download{
private: 
    Download() ; 
public:
    static Download *getInstance(); 
    static void destroyInstance(); 
    virtual ~Download(); 

    bool pushRequest(std::shared_ptr<DownloadRequest> &request); 

    bool downloadFile(std::shared_ptr<DownloadFile> &file);

    const std::shared_ptr<DownloadRequest> getDownloadList(const std::string &request_info_uid, const std::string &local_ip);
    const std::shared_ptr<DownloadRequest> getDownloadListParser(JsonNode *node, const std::string &arr_name);

    void cancel();
    bool cancel(std::string &request_info_uid);
    bool isWorking();
private: 
    void stop(); 

    bool pushFiles(const std::list<std::shared_ptr<DownloadFile> > &files);
    bool pushCheckFile(std::shared_ptr<DownloadFile> &file);

    void downloadRequestThreadFunc(); 
    void downloadFileThreadFunc();
    void downloadCheckThreadFunc();
    
    bool downloadRequest(std::shared_ptr<DownloadRequest> &request); 
    
    JsonNode* getNodeProgressInfo(std::shared_ptr<DownloadFile> &file);
    JsonNode* getNodeCompleteInfo(std::shared_ptr<DownloadRequest> &request);

    void sendResultEvent(DownloadStatus status, JsonNode *result_root_node, const char *event_name, gpointer user_data); 
    
private:
    static Download *s_instance ; 
    static std::atomic<bool> s_stop ; 
    static std::atomic<bool> s_cancel ;
    static std::atomic<bool> s_downloading ;

    std::thread _downloadRequestThread ; 
    std::thread _downloadFileThread ;
    std::thread _downloadCheckThread ;

    std::list<std::shared_ptr<DownloadFile> > _files ;
    std::list<std::shared_ptr<DownloadRequest> > _requests ; 
    std::list<std::shared_ptr<DownloadFile> > _checks ;

    std::mutex _downloadFileMutex ; 
    std::condition_variable _downloadFileCond ; 

    std::mutex _downloadRequestMutex ; 
    std::condition_variable _downloadRequestCond ; 

    std::mutex _downloadCheckMutex ; 
    std::condition_variable _downloadCheckCond ; 

    std::mutex _cancelMutex;
    std::condition_variable _cancelCond ;
}; 
}

END_FTC_CORE
#endif