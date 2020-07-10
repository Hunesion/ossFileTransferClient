#include <string>
#include "GlobalVar.h"
#include "../macro.h"
#include "../JsonUtils.h"

BEGIN_FTC_CORE

static ClientConfig s_ClientConfig ; 

namespace GlobalVar
{
    void init(){
        s_ClientConfig.init(); 
    }

    ClientConfig &getClientConfig(){
        return s_ClientConfig; 
    }

    const char *getFtcUrl() {
        return s_ClientConfig.getFtcUrl().c_str(); 
    }

    void dispose() {
        s_ClientConfig.dispose();
    }

} // namespace GlobalVar
END_FTC_CORE
