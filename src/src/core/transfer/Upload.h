#ifndef _FTC_CORE_TRANSFER_UPLOAD_H_
#define _FTC_CORE_TRANSFER_UPLOAD_H_
#include <string>
#include <list>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "UploadFile.h"
#include "../macro.h"
#include "../EventManager.h"
#include "../JsonUtils.h"

BEGIN_FTC_CORE namespace Transfer{

enum UploadStatus {
    UPLOAD_FAIL = 0,
    UPLOADING,
    UPLOADED,
    UPLOAD_CHECKING,
    UPLOAD_COMPLETE,
    UPLOAD_NONE
};

typedef struct _UploadResultParam {
    UploadStatus status;
    JsonNode *node;
    gpointer user_data;
} UploadResultParam;

class Upload{
private: 
    Upload() ; 
public:
    static Upload *getInstance(); 
    static void destroyInstance(); 
    virtual ~Upload(); 

    bool pushFile(const FileInfo & file, std::list<std::string> &networks); 
    bool pushFiles(const std::list<FileInfo>& files, std::list<std::string> &networks); 

    void setResultEvent(const char *eventName, void *userData);
    void cancel();
    bool isWorking();
    bool deleteUploadedFile(std::list<std::string> uploadedFilesList);

    static long long getFileContent(const char *fileName, long long offset , long long length , unsigned char **content);
private: 
    void stop(); 
    

    void uploadFilePartThreadFunc(); 
    void uploadThreadFunc(); 
    void uploadCheckThreadFunc();
    
    bool pushFileParts(const std::list<std::shared_ptr<UploadFilePart> > &parts); 
    bool uploadFilePart(std::shared_ptr<UploadFilePart> &part); 
    bool uploadFile(std::shared_ptr<UploadFile> &file); 
    bool uploadCheckBefore(std::list<std::shared_ptr<UploadFile>> &uncheckFiles);
    bool uploadCheck(std::list<std::shared_ptr<UploadFile>> &uncheckFiles);
    bool uploadCheckAfter(std::vector<std::shared_ptr<UploadFile>> &checkingFiles);

    static void uploadCheckResultEvent(Event *evt);
    std::shared_ptr<UploadFile> getFileFromFUID(std::vector<std::shared_ptr<UploadFile>> &files, std::string &compareFuid);
    bool uploadCheckResultParse(JsonNode *element, JsonObject *result_obj);

    JsonNode* getProgressResultNode(const char *filepath, int progress);
    JsonNode* getUploadFailResultNode(const char *msg);
    void sendResultEvent(UploadStatus status, JsonNode *result_root_node); 
    
    bool deleteUploadedFile(std::shared_ptr<UploadFile> deleteFile);
    bool deleteUploadedFile(std::list<std::shared_ptr<UploadFile>> &deleteFiles);
    bool deleteUploadedFile(std::vector<std::shared_ptr<UploadFile>> &deleteFiles);
private:
    static Upload *s_instance ; 
    static volatile bool s_stop ; 
    static bool s_cancel ;
    static bool s_uploading ;

    std::string _eventName;
    void *_userData;

    std::thread _uploadThread ; 
    std::thread _uploadCheckThread ;
    std::list<std::thread> _partSenderPool ; 
    std::list<std::shared_ptr<UploadFilePart>> _fileParts ; 
    std::list<std::shared_ptr<UploadFile> > _files ; 
    std::list<std::shared_ptr<UploadFile> > _checkFiles;
    std::vector<std::shared_ptr<UploadFile> > _checkingFiles;

    std::mutex _uploadMutex ; 
    std::condition_variable _uploadCond ; 

    std::mutex _partMutex ;
    std::condition_variable _partCond ;  

    std::mutex _uploadCheckMutex ;
    std::condition_variable _uploadCheckCond ;

    std::mutex _cancelMutex;
    std::condition_variable _cancelCond ;
}; 
}
END_FTC_CORE
#endif