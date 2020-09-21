
#include "CommonUtils.h"

BEGIN_FTC_CORE
namespace CommonUtils 
{
    void callMessageBoxEvent(const std::string &message)
    {
        std::string *param = NULL;
        EventManager *event_mgr = NULL;
        event_mgr = EventManager::getInstance();
        if (event_mgr == nullptr) {
            return;
        }

        param = new std::string(message);
        event_mgr->dispatchEventAsync(FTC_CORE_MESSAGE_BOX, (void*)param, [](void *release){
            if (release != nullptr) {
                delete (std::string*)release;
                release = nullptr;
            }
        });

        return;
    }

    void callNotificationEvent(const std::string &message)
    {
        std::string *param = NULL;
        EventManager *event_mgr = NULL;
        event_mgr = EventManager::getInstance();
        if (event_mgr == nullptr) {
            return;
        }

        param = new std::string(message);
        event_mgr->dispatchEventAsync(FTC_CORE_DESKTOP_NOTIFICATION, (void*)param, [](void *release){
            if (release != nullptr) {
                delete (std::string*)release;
                release = nullptr;
            }
        });

        return;
    }

    std::string getRandomString(int length)
    {
        static const char alphanum[] = "0123456789" 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
        "abcdefghijklmnopqrstuvwxyz";
        char *random = NULL;
        std::string rv;
        
        random = new char[length + 1];
        memset(random, 0, length + 1);

        srand(time(0));

        for (int i = 0; i < length; ++i) {
            random[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        } 

        rv = random;
        
        if (random) {
            delete[] random;
            random = NULL;
        }

        return rv;
    }
}
END_FTC_CORE