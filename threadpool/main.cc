#include "Task.hpp"
#include "threadpool.hpp"
#include <mermory>
#include <time.h>

int main()
{
  srand(time(nullptr) ^ getpid());
  EnableConsoleLogStrategy();

  std::unique_ptr<ThreadPool<Task>> tp  = std::make_unique<ThreadPool<Task>>(10);
  //std::unique_ptr<ThreadPool<Task>> tp2 = std::make_unique<ThreadPool<Task>>(10);
  //std::unique_ptr<ThreadPool<Task>> tp3 = std::make_unique<ThreadPool<Task>>(10);

  tp->Start();
  int cnt = 10;
  while(cnt--)
  {
    int x=rand()%10+1;
    usleep(rand()%73);
    int y=rand()%5+1;
    Task t(x,y);

    tp->Enqueue(t);
    sleep(1);
  }

  tp->Stop();
  tp->Wait();

  // tp->Enqueue();

  return 0;
}