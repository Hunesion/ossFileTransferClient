#ifndef _FTC_CORE_LOGGER_H_
#define _FTC_CORE_LOGGER_H_

#include <stdarg.h>
#include "macro.h"

#define FTC_LOG(format, ... ) Ftc::Core::Logger::log(format , ##__VA_ARGS__)

#ifdef FTC_DEBUG
#define FTC_LOG_DEBUG(format , ... )  Ftc::Core::Logger::log(format , ##__VA_ARGS__)
#else 
#define FTC_LOG_DEBUG( ... ) do{}while(0)
#endif

BEGIN_FTC_CORE
namespace Logger{
    void log(const char *format , ... ); 
}
END_FTC_CORE


#endif