#ifndef _FTC_CORE_MODEL_GLOBAL_VAR_H_
#define _FTC_CORE_MODEL_GLOBAL_VAR_H_
#include <string>
#include <json-glib/json-glib.h>

#include "ClientConfig.h"
#include "ValueObject.h"


BEGIN_FTC_CORE
namespace GlobalVar
{
    void init(); 
    void setFtcUrl(const char *url);
    const char *getFtcUrl();
    ClientConfig &getClientConfig(); 
    void dispose();
} // namespace GlobalVar

END_FTC_CORE

#endif /* _FTC_CORE_MODEL_GLOBAL_VAR_H_ */