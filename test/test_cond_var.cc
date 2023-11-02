#include <unistd.h>

#include <atomic>
#include <iostream>
#include <vector>

#include "cond_variable.h"
#include "lock.h"
#include "mutex.h"
#include "sem.h"
#include "thread.h"

class Task {
public:
  Task() = default;
  Task(int id) : taskId(id) {}
  void doTask() {
    std::cout << "Task threadId: " << far::this_thread::get_id() << '\n';
    std::cout << "doTask taskId: " << taskId << '\n';
  }

private:
  int taskId;
};

std::vector<Task> tasks;
static int count = 0;
static int conusem = 0;
far::mutex tmutex;
far::conditional_variable cond;
far::sem sem{0};

void addTask(int num) {
  while (num > 0) {
    {
      far::lock_guard<far::mutex> lock(tmutex);
      tasks.emplace_back(++count);
      --num;
    }
    cond.notfiy_one();
  }
}

void consumeTask(int num) {
  while (true) {
    Task task;
    {
      far::lock_guard<far::mutex> lock(tmutex);
      cond.wait_for(
          lock, [](int cnt) { return !tasks.empty() && conusem < cnt; }, num);
      ++conusem;
      if (conusem > num) {
        cond.notify_all();
        break;
      }
      task = tasks.back();
      tasks.pop_back();
    }
    task.doTask();
    sleep(1);
  }
}

int main(int argc, char *argv[]) {
  far::thread t(addTask, 20);
  far::thread t1(consumeTask, 20);
  far::thread t2(consumeTask, 20);
  t.join();
  t1.join();
  t2.join();
  return 0;
}
