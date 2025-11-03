#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <functional>
#include <sys/syscall.h>
#include "Logger.hpp"

#define get_lwp_id() syscall(SYS_gettid)

using func_t = std::function<void(const std::string &name)>;
const std::string threadnamedefault = "None-Name";

class Thread
{
  public:
    Thread(func_t func,const std::string &name = threadnamedefault)
      :_name(name),
       _func(func),
       _isrunning(false)
    {
      LOG(LogLevel::INFO)<<_name<<"create thread obj success";
    }

    static void*start_routine(void *args)
    {
      Thread *self = static_cast<Thread *>(args);
      self->_isrunning = true;
      self->_lwpid = get_lwp_id();
      self->_func(self->_name);
      pthread_exit((void *)0);
    }

    void Start()
    {
      int n = pthread_create(&_tid,nullptr,start_routine,this);
      if(n==0)
      {
        LOG(LogLevel::INFO) << _name <<"running success";
      }
    }

    void Stop()
    {
      int n = pthread_cancel(_tid);
      (void)n;
    }

    void Join()
    {
      if(!_isrunning)
        return;
    
      int n = pthread_join(_tid,nullptr);
      if(n==0)
      {
        LOG(LogLevel::INFO)<<_name<<"pthread_join success";
      }
    }

    ~Thread()
    {
      //LOG(LogLevel::INFO) << _name <<"destory thread obj success";
    }

  private:
    bool _isrunning;
    pthread_t _tid;
    pid_t _lwpid;
    std::string _name;
    func_t _func;
};

#endif