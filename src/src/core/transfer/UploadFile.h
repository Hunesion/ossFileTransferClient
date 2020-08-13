#ifndef _FTC_CORE_TRANSFER_UPLOAD_FILE_H_
#define _FTC_CORE_TRANSFER_UPLOAD_FILE_H_
#include <string>
#include <list>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "../macro.h"
#include "../FileInfo.h"
BEGIN_FTC_CORE namespace Transfer{

class UploadFile ; 
 
class UploadFilePart{
public: 
    UploadFilePart(); 
    UploadFilePart(std::shared_ptr<UploadFile> file, long long offset , long long length , long long fileLength); 
    virtual ~UploadFilePart(); 
    FTC_SYNTHESIZE_READONLY(long long , _fileLength, FileLength)
    FTC_SYNTHESIZE_READONLY(long long , _offset , Offset)
    FTC_SYNTHESIZE_READONLY(long long , _length , Length) 
    UploadFile *getFile() const ; 
    std::shared_ptr<UploadFile> &getFilePtr() ; 
    void setCompleted(); 
    bool isCompleted() const ; 
    bool isEquals(const UploadFilePart &other); 
private: 
    std::shared_ptr<UploadFile> _file; 
    bool _complete; 
};

class UploadFile{
public:
    UploadFile(const FileInfo& info, std::list<std::string> &networks); 
    virtual ~UploadFile(); 
    const FileInfo &getFileInfo() const ; 
    const std::string & getServerFileName() const ; 
    const std::string & getServerFileUid() const;
    void setServerFileName(const std::string &name); 
    void setServerFileUid(const std::string &uid);
    const std::list<std::shared_ptr<UploadFilePart>> &getFileParts() const; 
    bool isUploadCompleted();
    bool isCheckCompleted();
    bool clearFilePart();  

    int  getCompleteFilePartCount() const;
    bool setCompleteFilePart(std::shared_ptr<UploadFilePart> &filePart); 
    void setFilePartSize(std::shared_ptr<UploadFile> file , long long partSize); 
    void setCheckComplete();

    bool isUploadDone(); 

    std::string toNetworkParam();
    std::string toFileParam();

private: 
    void wakeUp(); 

private: 
    FileInfo _fileInfo ;

    bool _uploadComplete ;
    bool _checkComplete ; 
    std::string _msg;

    std::string _serverFileName ; 
    std::string _serverFileUid;
    std::list<std::string> _networks;
    std::list<std::shared_ptr<UploadFilePart>> _parts ; 
    int _completeCount ;
    std::mutex _lock ; 
    std::condition_variable _cond ; 
}; 
 

}
END_FTC_CORE
#endif 