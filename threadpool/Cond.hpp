#pragma once

#include <iostream>
#include <pthread.h>
#include "Mutex.hpp"

class Cond
{
 public:
    Cond()
    {
      pthread_cond_init(&_cond,nullptr);
    }
    
    void Wait(Mutex &lock)
    {
      int n=pthread_cond_wait(&_cond,lock.Get());
    }

    void NotifyOne()
    {
      int n = pthread_cond_signal(&_cond);
      (void)n;
    }

    void NotifyAll()
    {
      int n = pthread_cond_broadcast(&_cond);
      (void)n;
    }
    
    ~Cond()
    {
      pthread_cond_destroy(&_cond);
    }
 private:
    pthread_cond_t _cond;
};