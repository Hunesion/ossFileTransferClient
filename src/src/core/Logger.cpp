#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include "Logger.h"
#include "FileInfo.h"
#include "TimeUtils.h"
#include "FileUtils.h"
#include "model/GlobalVar.h"

#define MAX_LOG_LENGTH (16 * 1024)

static std::mutex s_mtx;

BEGIN_FTC_CORE
namespace Logger{

    void logWrite(const char *log) {
        std::unique_lock<std::mutex> lk(s_mtx);
        ClientConfig &clientConfig = GlobalVar::getClientConfig();
        FileInfo fiConfig = FileInfo::getFileInfo(clientConfig.getConfigDirPath().c_str());
        FileInfo fiLog;
        std::stringstream ss;
        std::string filepath, writeData;
        std::tm tm = TimeUtils::now();
        
        if (! fiConfig.isExist()) {
            return;
        }

        fiLog = FileInfo::getFileInfo(std::string(fiConfig.getFullPath() + "/log").c_str());

        if (! fiLog.isExist()) {
            if (mkdir(fiLog.getFullPath().c_str(), 0755) != 0) {
                return;
            }
        }

        //  파일이름
        ss << fiLog.getFullPath();
        ss << "/";
        ss << "FTC_";
        ss << TimeUtils::getDateFormat(tm, "%Y-%02m-%02d");
        ss << ".txt";
        filepath = ss.str();
        ss.str("");
    
        //  로그포멧
        ss << "[";
        ss << TimeUtils::getDateFormat(tm, "%Y-%02m-%02d %H:%M:%S");
        ss << "] ";
        ss << "[";
        ss << "0x" << std::uppercase << std::setfill('0') << std::setw(sizeof(unsigned long)) << std::hex << pthread_self(); /*thread id*/
        ss << "] ";
        ss << log;
        writeData = ss.str();

        //  저장
        FileUtils::fileWrite(filepath.c_str(), (char*)writeData.c_str(), strlen(writeData.c_str()), 2);
    }

    void log(const char * format, ...)
    {
        int bufferSize = MAX_LOG_LENGTH;
        char* buf = nullptr;
        int nret = 0;
        va_list args;

        do
        {
            buf = new (std::nothrow) char[bufferSize];
            if (buf == nullptr)
                return;
            va_start(args, format);
            nret = vsnprintf(buf, bufferSize - 3, format, args);
            va_end(args);

            if (nret >= 0)
            { 
                if (nret <= bufferSize - 3)
                {
                    break;
                }
                else
                {
                    bufferSize = nret + 3;
                    delete[] buf;
                }
            }
            else 
            {	
                bufferSize *= 2;
                delete[] buf;
            }
        } while (true);
        buf[nret] = '\n';
        buf[++nret] = '\0';

#ifdef FTC_DEBUG
        fprintf(stdout, "%s", buf);
        fflush(stdout);
#endif
        logWrite(buf);


        delete[] buf;
    }
}
END_FTC_CORE