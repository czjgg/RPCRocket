#ifndef ROCKET_COMMOM_LOG_h
#define ROCKET_COMMOM_LOG_h

#include <string>

#include "rocket/common/config.h"
#include "rocket/commom/mutex.h"
#include "rocket/net/timer_event"

namespace rocket{


  template<typename... Args>
  std::string formatString(const char* str, Args&&... args){
    int size = snprintf(nullptr,0,str,args...);
    std::string result;
    if(size > 0){
      snprintf(result,size+1,str,args);
    }
    return result;
  }


  #define DEBUGLOG(str,...)\
    if(rocket::Logger::GetGlobalLogger()->getLogLevel() && rocket::Logger::GetGlobalLogger()->getLogLevel() <= Debug)\
    {\
      rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::Loglevel::Debug).toString()\
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE___) + "]\t" + rocket::formatString(str, ##__VA_ARGS__)+"\n");\
    }\
  
  #define INFOLOG(str,...)\
    if(rocket::Logger::GetGlobalLogger()->getLogLevel() && rocket::Logger::GetGlobalLogger()->getLogLevel() <= Info)\
    {\
      rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::Loglevel::Info).toString()\
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE___) + "]\t" + rocket::formatString(str, ##__VA_ARGS__)+"\n");\
    }\
  
  #define ERRORLOG(str,...)\
    if(rocket::Logger::GetGlobalLogger()->getLogLevel() && rocket::Logger::GetGlobalLogger()->getLogLevel() <= Error)\
    {\
      rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::Loglevel::Error).toString()\
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE___) + "]\t" + rocket::formatString(str, ##__VA_ARGS__)+"\n");\
    }\
  
  #define APPDEBUGLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Debug) \
    { \
      rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Debug).toString() \
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
    } \
  #define APPINFOLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Info) \
    { \
      rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Info).toString() \
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
    } \

  #define APPERRORLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Error) \
    { \
      rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Error).toString() \
      + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
    } \

  
  enum LogLevel{
    Unknown = 0,
    Debug = 1,
    Info = 2,
    Error = 3
  };
  std::string LogLevelToString(LogLevel level);
  LogLevel StringToLogLevel(const std::string& log_level);

  class Logger{
    public:
    typedef std::shared_ptr<Logger> s_ptr;
    
    Logger(LogLevel level, int type = 1);

    void pushLog(const std::string& msg);

    void pushAppLog(const std::string& msg);

    void init();

    void log();

    Loglevel getLogLevel()const{
      return m_set_level;
    }

    void syncLoop();

    public:

    static Logger* GetGlobalLogger();

    static void InitGlobalLogger(int type = 1);

    private:

      Loglevel m_set_level;
      std::vector<std::string> m_buffer;

      std::vector<std::string> m_app_buffer;

      Mutex m_mutex;

      Mutex m_app_mutex;

      // m_file_path/m_file_name_yyyymmdd.1

      std::string m_file_name;    // 日志输出文件名字
      std::string m_file_path;    // 日志输出路径
      int m_max_file_size {0};    // 日志单个文件最大大小

      AsyncLogger::s_ptr m_asnyc_logger;

      AsyncLogger::s_ptr m_asnyc_app_logger;

      TimerEvent::s_ptr m_timer_event;

      int m_type {0};
  }

  class LogEvent{
    public:
      LogEvent(LogLevel level) : m_level(level){}

      std::string getFileName()const{
        return m_file_name;
      }

      LogLevel getLogLevel() const{
        return m_level;
      }

      std::string toString();
    private:
      int32_t m_file_line;
      int32_t m_pid;
      int32_t m_thread_id;
      int32_t m_fiberId = 0;//协程id，后期拓展

      LogLevel m_level;

      std::string m_file_name;

      
  }
}