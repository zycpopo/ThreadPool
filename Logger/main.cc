#include <unistd.h>
#include "log.hpp"

int main()
{
  EnableConsoleLogStrategy();

    LOG(LogLevel::WARNING) << "hello world" << ", 3.14 " << 123;
    LOG(LogLevel::ERROR) << "hello world" << ", 3.14 " << 123;
    LOG(LogLevel::ERROR) << "hello world" << ", 3.14 " << 123;
}