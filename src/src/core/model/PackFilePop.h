#ifndef _FTC_CORE_MODEL_PACKFILEPOP_H_
#define _FTC_CORE_MODEL_PACKFILEPOP_H_

#include "../macro.h"
#include "ValueObject.h"
#include "GlobalStruct.h"
#include <list>
#include <set>

BEGIN_FTC_CORE

class PackFilePop : public BaseValue
{
public : 
    PackFilePop(const std::string& network_id) ; 
    virtual ~PackFilePop(); 
    JsonObject *getApproval(); 
    void setApproval(JsonObject *obj) ;
    

    void putRecvNetwork(const char* network); 
    void removeRecvNetwork(const char* network); 
    void copyApprovalJson(JsonObject* dest); 

    JsonObject *getApprovalData();

protected : 
    virtual bool parseJsonNode(JsonNode* node) override; 
    virtual bool internalClear() override ; 

    enum NetworkPolicyCheckCode
    {
        NONE = 0,
        ALLOW_WEEKEND_PERIOD = 1,
        ALLOW_TIME_PERIOD = 2,
        FT_PERMIT_YN = 4,
        FT_PERMIT_SEND = 8
    };

    JsonObject *getPolicySettingUserVO(const char* network); 
    std::list<JsonObject*> getApprovalUserVO(const char* network); 

    bool generateDefaultApproval(); 
    bool putNetowkrApproval(JsonObject *data , const char* network); 
    bool putNetowkrApproval(const char* network); 
    bool removeNetworkApproval(const char* network); 
    void setReceiveNetwork(); 

    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(bool, _is_my_exception, IsMyException); /* 결재예외 */
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(bool, _is_temp_use_exception, IsTempUseException); /* 임시결재 예외 */
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(bool, _is_approval_officer_yn, IsApprovalOfficerYN); /* 임원여부 */
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(int, _use_apt_deny_file_size, UseAptDenyFileSize);
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(int, _apt_detect_file_size, AptDectectFileSize);
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(bool, _apt_detect_use, AptDetectUse); 
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(int, _receive_id_sum, ReceiveIdSum); 
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(bool, _retain_id_sum, RetainIdSum); 
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, _ftc_contents, FtcContents);
    FTC_SYNTHESIZE_READONLY_PASS_BY_REF(std::list<GlobalStruct::FtcNetwork>, _list_ftc_network, ListFtcNetwork);
    FTC_SYNTHESIZE_PASS_BY_REF(int, _dynamic_file_policy_type, DynamicFilepolicyType);
    FTC_SYNTHESIZE_PASS_BY_REF(int, _file_cnt_with_zip, FileCntWithZip);
    FTC_SYNTHESIZE(bool, _is_violate_file, IsViolateFile);
    FTC_SYNTHESIZE(bool, _is_private_info, IsPrivateInfo);
    FTC_SYNTHESIZE(bool, _is_approval_line_extension_check, IsApprovalLineExtensionCheck)
private :
    JsonObject *_obj ; 
    JsonObject *_obj_info;
    JsonObject *_obj_policy_info;
    JsonObject *_approval; 

    std::set<std::string> _recv_network ; 
    std::string _network_id ; 
    std::string _user_id ; 
    std::string _user_name ; 
    bool _server_approval ; 
};



END_FTC_CORE


#endif /* _FTC_CORE_MODEL_PACKFILEPOP_H_ */