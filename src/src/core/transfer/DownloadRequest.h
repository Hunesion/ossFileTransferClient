#ifndef _FTC_CORE_TRANSFER_DOWNLOAD_REQUEST_H_
#define _FTC_CORE_TRANSFER_DOWNLOAD_REQUEST_H_
#include <string>
#include <list>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "../macro.h"
#include "../FileInfo.h"
BEGIN_FTC_CORE namespace Transfer{

class DownloadRequest ; 
 
class DownloadFile{
public: 
    DownloadFile();
    DownloadFile(std::shared_ptr<DownloadRequest> request, std::string &request_info_file_list_uid, std::string &filename, std::string &from_network_id, std::string &to_network_id, std::string &list_type); 
    virtual ~DownloadFile(); 

    const std::string& getRequestInfoFileListUid() const;
    const std::string& getFilename() const;
    void setFilename(const std::string &filename);
    const std::string& getRelativeDir() const;
    void setRelativeDir(const std::string &relative_dir);

    DownloadRequest *getRequest() const ; 
    std::shared_ptr<DownloadRequest> &getRequestPtr() ; 
    const std::string& getFromNetworkId() const;
    const std::string& getToNetworkId() const;
    const std::string& getListType() const;

    const std::string& getErrorMsg() const;
    void setErrorMsg(const std::string &error_msg);

    int getProgress();
    void setProgress(int progress);

    int getLength();
    void setLength(int length);

    void setCancel();
    bool isCancel() const;

    void setCompleted();
    bool isCompleted() const ;

    void setSuccess();
    bool isSuccess() const ;

    bool isEquals(const DownloadFile &other);
private: 
    std::string _request_info_file_list_uid;
    std::string _filename;
    std::string _relative_dir;
    std::string _from_network_id;
    std::string _to_network_id;
    std::string _list_type;

    std::string _error_msg;

    std::shared_ptr<DownloadRequest> _request; 
    std::atomic<int> _progress;
    std::atomic<bool> _complete; 
    std::atomic<bool> _success; 
    std::atomic<bool> _cancel;

    int _length;

    std::mutex _lock ; 
};

class DownloadRequest{
public:
    DownloadRequest();
    DownloadRequest(std::string &request_info_uid, std::string &directory);
    DownloadRequest(std::string &request_info_uid, std::string &directory, std::list<std::shared_ptr<DownloadFile>> files); 

    virtual ~DownloadRequest(); 

    void pushDownloadFile(std::shared_ptr<DownloadFile> &file);
    
    const std::string& getRequestInfoUid() const;
    const std::string& getDirectory() const;
    const std::list<std::shared_ptr<DownloadFile>> &getDownlaodFiles() const; 
    const std::string& getEventName() const;
    void* getUserData() const;

    void setRequestInfoUid(const std::string &request_info_uid);
    void setDirectory(const std::string &directory);
    void setEvent(const char *event_name, void *user_data);

    bool isDownloadCompleted();
    bool clearFiles();
    bool clearFile(const Ftc::Core::Transfer::DownloadFile *down_file);

    int  getDownloadFilesCount() const;
    int  getCompleteDownloadCount() const;
    
    bool setCompleteDownloadFile(std::shared_ptr<DownloadFile> &file); 

    bool isDownloadDone(); 

private: 
    void wakeUp(); 

private: 
    std::string _request_info_uid;
    std::string _directory;
    std::string _event_name;
    void *_user_data;
    
    std::list<std::shared_ptr<DownloadFile>> _files ; 

    bool _downloadComplete ;
    int _completeCount ;

    std::mutex _lock ; 
    std::condition_variable _cond ; 
}; 
}
END_FTC_CORE
#endif 