#ifndef _FTC_CORE_URL_REDIRECTION_H_
#define _FTC_CORE_URL_REDIRECTION_H_

#include "macro.h"
#include <string>


BEGIN_FTC_CORE

bool openUrl(const char *url);

bool loadUrl(const char *filepath);

bool sendUrl(const char *url);

END_FTC_CORE
#endif /* _FTC_CORE_URL_REDIRECTION_H_ */
