#pragma once
#include <iostream>
#include <functional>
#include <unistd.h>
#include <sstream>

class Task
{
public:
    Task()
    {}
    Task(int x,int y):a(x),b(y)
    {

    }
    void Execute()
    {
      result=a+b;
    }
    void operator()()
    {
      Execute();
    }
    void Print()
    {
      std::cout<<a<<" + "<<b<<" = "<<result<<std::endl;
    }
    std::string ResulttoString()
    {
      std::stringstream ss;
      ss<<a<<" + "<<b<<" = "<<result;
      return ss.str();
    }
private:
    int a;
    int b;
    int result;
};