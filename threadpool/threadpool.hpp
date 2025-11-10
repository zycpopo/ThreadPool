#pragma once

#include <iostream>
#include <unistd.h>
#include <queue>
#include <vector>
#include <cstdio>

#include "Mutex.hpp"
#include "Cond.hpp"
#include "thread.hpp"

template<class T>
class ThreadPool
{
  private:
      Bool QueuepIsEmpty()
      {
        return _q.empty();
      }

      void Rountine(const std::string &name)
      {
        while(true)
        {
          T t;
          {
            LockGuard lockguard(&_lock);
            while(QUeueIsEmpty()&&_is_running)
            {
              _wait_thread_num++;
              _cond.Wait(_lock);
              _wait_thread_num--;
            }
            if(!_is_running && QueueEmpty())
            {
              LOG(LogLevel::INFO)<<"线程池退出 && 任务队列为空";
              break;
            }

            t=_q.front();
            _q.pop();
          }
          t();
          LOG(LogLevel::DEBUG) << name <<"handler task: "<<t.Resulttostring();
        }

      }
  public:
      ThreadPool(int threadnum = defaultthreadnum)
        : _threadnum(threadnum), _is_running(false), _wait_thread_num(0)
    {
        for (int i = 0; i < _threadnum; i++)
        {
            // 方法1:
            // auto f = std::bind(hello, this);
            // 方法2
            std::string name = "thread-" + std::to_string(i + 1);
            _threads.emplace_back([this](const std::string &name)
                                  { this->Routine(name); }, name);

            // Thread t([this](){
            //     this->hello();
            // }, name);
            // _threads.push_back(st::move(t));
        }
        LOG(LogLevel::INFO) << "thread pool obj create success";
    }

      void Start()
      {
        if(_is_running)
        {
          return;
        }
        _is_running = true;
        for(auto &t : _threads)
        {
           t.Start();
        }
        LOG(LogLevel:INFO)<<"threadpool running success";
      }

      void Stop()
      {
        if(!_is_running)
          return;
        
        _is_running=false;
        if(_wait_thread_num)
          _cond.NotifyAll();
      }

      void Wait()
      {
        for(auto &t:_threads)
        {
          t.Join();
        }
        LOG(LogLevel::INFO)<<"thread pool wait success";
      }

      void Enqueue(const T &t)
      {
        if(!_is_running)
          return;

        {
          LockGuard lockguard(&_lock);

          _q.push(t);
          if(_wait_thread_num>0)
          {
            _cond.NotifyOne();
          }
        }
      }

      
  private:
      std::queue<T> _q;
      std::vector<Thread>;
      int _threadnum;
      int _wait_thread_num;

      Mutex _mutex;
      Cond _cond;

      bool _is_running;

};