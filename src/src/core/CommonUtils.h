#ifndef _FTC_COMMON_UTILS_H_
#define _FTC_COMMON_UTILS_H_

#include <string>
#include "core.h"

BEGIN_FTC_CORE
namespace CommonUtils 
{
    void callMessageBoxEvent(const std::string &message);

    void callNotificationEvent(const std::string &message);

    std::string getRandomString(int length);
}
END_FTC_CORE

#endif /* _FTC_COMMON_UTILS_H_ */