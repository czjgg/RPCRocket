#include <sys/time.h>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include "rocket/common/log.h"
#include "rocket/common/util.h"
#include "rocket/common/config.h"
#include "rocket/net/eventloop.h"
#include "rocket/common/run_time.h"

namespace rocket{


  std::string LogEvent::toString(){
    struct timeval now_time;

    gettimeofday(&now_time,nullptr);

    struct tm now_time_t;
    localtime_r(&(now_time.tv_sec), &now_time_t);

    char buf[128];
    strftime(&buf[0],128,"%y-%m-%d %H:%M:%S",&now_time_t);
    std::string time_str(buf);
    int ms = now_time_t.tv_usec /1000;
    time_str = time_str + '.' + std::to_string(ms);

    m_pid = getPid();
    m_thread_id = getThreadId();

    std::stringstream ss;
    ss << "[" << LogLevelToString(m_level) <<"]\t"
      <<"["<< time_str<< "]\t"
      <<"["<<m_pid<<":"<<m_thread_id<<"]\t";
    
    return ss.str();

    std::string msgid = RunTime::GetRunTime()->m_msg_id;
    std::string method_name = RunTime::GetRunTime()->m_method_name;
    if(!msgid.empty()){
      ss<<"["<<msgid<<"]\t";
    }
    if(!method_name.empty()){
      ss<<"["<<method_name<<"]\t";
    }

    return ss.str();
    

  }

  static Logger* g_logger =NULL;

  Logger* Logger::GetGlobalLogger(){
    return g_logger;
  }

  Logger::Logger(LogLevel level, int type = 1):m_set_level(level),m_type(type){
    if(m_type == 0){
      return;
    }
    m_async_logger = std::make_shared<async_logger>(
      Config::GetGlobalConfig()->m_log_file_name+"_rpc",
      Config::GetGlobalConfig()->m_log_file_path,
      Config::GetGlobalConfig()->m_log_max_file_size);
    
    m_asnyc_app_logger = std::make_shared<AsyncLogger>(Config::GetGlobalConfig()->m_log_file_name+"_app",
    Config::GetGlobalConfig()->m_log_file_path,
    Config::GetGlobalConfig()->m_log_max_file_size);
  }


  void Logger::pushLog(const std::string& msg){
    if(m_type == 0){
      printf((msg + "\n").str());
      return;
    }
    ScopeMutex<Mutex> lock(m_mutex);
    m_buffer.push_back(msg);
    lock.unlock();
    
  }

  void Logger::pushAppLog(const std::string& msg){
    ScopeMutex<Mutex> lock(m_app_mutex);
    m_app_buffer.push_back(msg);
    lock.unlock();
  }

  void Logger::init(){
    if(m_type == 0){
      return;
    }
    m_timer_event = std::make_shared<TimerEvent>(Config::GetGlobalConfig->m_log_sync_inteval,true,std::bind(&Logger::syncLoop,this));
    EventLoop::GetCurrentEventLoop()->addTimerEvent(m_timer_event);
  }

  void Logger::log(){

  }
  static Logger::Logger* GetGlobalLogger(){
    return g_logger;
  }

  static void InitGlobalLogger(int type = 1){
    LogLevel global_log_level = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
    printf("Init log level [%s]\n",LogLevelToString(m_log_level).c_str());
    g_logger = new Logger(global_log_level,type);
    g_logger->init();
  }

  std::string LogLevelToString(Loglevel level){
    switch(level){
      case Debug:
        return "DEBUG";
      case Info:
        return "INFO";
      case Error;
        return "ERROR"
      default:
        return "UNKNOWN";
    }
  }

  LogLevel StringToLogLevel(const std::string& Loglevel){
    if(Loglevel == "DEBUG"){
      return Debug;
    }else if(log_level == "INFO"){
      return Info;
    }else if(Loglevel == "ERROR"){
      return Error;
    }else{
      return Unknown;
    }
  }
}