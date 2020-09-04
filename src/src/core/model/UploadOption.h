#ifndef _FTC_CORE_MODEL_UPLOAD_OPTION_H_
#define _FTC_CORE_MODEL_UPLOAD_OPTION_H_
#include "GlobalStruct.h"
#include "../JsonUtils.h"
#include <string>
#include <vector>
#include <map>
BEGIN_FTC_CORE

#define JSON_KEY_APPROVAL_LEVEL "approval_level_%d_%s"
#define JSON_KEY_APPROVAL_LEVEL_NAME "approval_level_name_%d_%s"
#define JSON_KEY_APPROVAL_USER_ID "approval_user_id_%d_%s"
#define JSON_KEY_APPROVAL_USER_NAME "approval_user_name_%d_%s_%s"
#define JSON_KEY_IS_AFTER_CHECK "is_after_check_%d_%s"
#define JSON_KEY_IS_AFTER_CHECK_USER "is_after_check_user_%d_%s"

class UploadOption
{
public:
    UploadOption();
    virtual ~UploadOption();

private:
    std::vector<GlobalStruct::FtcNetwork> vc_network_list;
    std::vector<GlobalStruct::FtcUser> vc_user_list;
    std::map<std::string, std::string> map_approval_lines;
    std::string send_title;
    std::string send_reason;
    std::string privacy_info;

    static void parseApprovalLines (JsonObject *object, const gchar *memberName, JsonNode *memberNode, gpointer userData) ;
public:
    void setNetworks(std::vector<GlobalStruct::FtcNetwork> &vc_network_list);
    void addNetwork(GlobalStruct::FtcNetwork &network);
    int findNetworkId(const char *network_id);
    int findNetworkName(const char *network_name);
    GlobalStruct::FtcNetwork& getNetwork(unsigned int index);
    unsigned int getNetworkSize();
    void clearNetwork();

    void addUser(GlobalStruct::FtcUser &user);
    int findUser(const char *user_id);
    GlobalStruct::FtcUser& getUser(unsigned int index);
    unsigned int getUserSize();
    void clearUser();
    std::string toUserParam();

    const char* getSendReason();
    void setSendReason(std::string &reason);
    void setSendReason(const char *reason);
    void clearSendReason();

    const char* getSendTitle();
    void setSendTitle(std::string &title);
    void setSendTitle(const char *title);
    void clearSendTitle();

    void setApprovalLines(std::map<std::string, std::string> &approval_lines);
    void setApprovalLines(JsonObject *obj);
    const std::map<std::string, std::string>& getApprovalLines();
    void toApprovalJsonFormat(JsonObject *json_obj);
    void clearApprovalLines();
};

END_FTC_CORE




#endif /* _FTC_CORE_MODEL_UPLOAD_OPTION_H_ */