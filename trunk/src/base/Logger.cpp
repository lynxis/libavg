//
// $Id$
//

#include "Logger.h"

#include <sys/time.h>
#include <fstream>
#include <iomanip>
#include <syslog.h>

using namespace std;

namespace avg {

const long Logger::NONE=0;
const long Logger::BLTS=1;
const long Logger::PROFILE=2;
const long Logger::PROFILE_LATEFRAMES=4;
const long Logger::EVENTS=8;
const long Logger::EVENTS2=16;
const long Logger::CONFIG=32;  
const long Logger::WARNING=64;
const long Logger::ERROR=128;  
const long Logger::MEMORY=256;
const long Logger::APP=512;
const long Logger::LOGGER=1024;

Logger* Logger::m_pLogger = 0;

Logger * Logger::get()
{
    if (!m_pLogger) {
        m_pLogger = new Logger;
    }
    return m_pLogger;
}

Logger::Logger()
{
    m_Flags = ERROR | WARNING | APP | LOGGER;
    m_DestType = CONSOLE;
    m_pDest = &cerr;
    trace(LOGGER, "Logging started ");
}

Logger::~Logger()
{
    closeDest();
}

void Logger::setConsoleDest()
{
    closeDest();
    m_DestType = CONSOLE;
    m_pDest = &cerr;
    AVG_TRACE(LOGGER, "Logging started ");
}

void Logger::setFileDest(const std::string& sFName)
{
    closeDest();
    m_DestType = FILE;
    m_pDest = new ofstream(sFName.c_str(), ios::out | ios::app);
    if (!*m_pDest) {
        m_pDest = &cerr;
        m_DestType = CONSOLE;
        AVG_TRACE(LOGGER, "Could not open " << sFName 
                << " as log destination.");
    } else {
        AVG_TRACE(LOGGER, "Logging started ");
    }
}

void Logger::setSyslogDest(int facility, int logopt)
{
    closeDest();
    m_DestType = SYSLOG;
    openlog("libavg", logopt, facility);
}
    
void Logger::setCategories(int flags)
{
    m_Flags = flags | ERROR | APP;
}

void Logger::trace(int category, const std::string& msg)
{
    if (category & m_Flags) {
        if (m_DestType == CONSOLE || m_DestType == FILE) {
            struct timeval time;
            gettimeofday(&time, NULL);
            struct tm* pTime;
            pTime = localtime(&time.tv_sec);
            char timeString[256];
            strftime(timeString, sizeof(timeString), "%y-%m-%d %H:%M:%S", pTime);

            (*m_pDest) << "[" << timeString << "." << 
                setw(3) << setfill('0') << time.tv_usec/1000 << setw(0) << "] ";
            (*m_pDest) << categoryToString(category) << ": ";
            (*m_pDest) << msg << endl;
        } else {
            int prio;
            switch(category) {
                case EVENTS:
                case EVENTS2:
                case BLTS:
                    prio = LOG_INFO;
                    break;
                case PROFILE:
                case PROFILE_LATEFRAMES:
                case CONFIG:
                case APP:
                case LOGGER:
                    prio = LOG_NOTICE;
                    break;
                case WARNING:
                    prio = LOG_WARNING;
                    break;
                case ERROR:
                    prio = LOG_ERR;
                    break;
                default:
                    prio = LOG_ERR;
            }
            syslog(prio, "%s: %s", categoryToString(category),
                    msg.c_str());
        }
    }
}

const char * Logger::categoryToString(int category)
{
    switch(category) {
        case BLTS:
            return "BLIT";
        case PROFILE:
        case PROFILE_LATEFRAMES:
            return "PROFILE";
        case EVENTS:
        case EVENTS2:
            return "EVENTS";
        case CONFIG:
            return "CONFIG";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case APP:
            return "APP";
        case LOGGER:
            return "LOGGER";
        default:
            return "UNKNOWN";
    }
}

void Logger::closeDest()
{
    switch (m_DestType) {
        case CONSOLE:
            break;
        case FILE:
            delete m_pDest;
            m_pDest = 0;
            break;
        case SYSLOG:
            closelog();
            
    }
}

}
