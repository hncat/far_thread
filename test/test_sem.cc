#include <unistd.h>

#include <atomic>
#include <iostream>
#include <vector>

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
far::mutex tmutex;
far::atomic_lock tamutex;
far::sem tsem{0};

void addTask(int num) {
  while (num > 0) {
    far::lock_guard<far::atomic_lock> lock(tamutex);
    tasks.emplace_back(++count);
    tsem.post();
    --num;
  }
}

void consumeTask() {
  while (true) {
    if (!tsem.wait()) {
      continue;
    }
    Task task;
    {
      far::lock_guard<far::atomic_lock> lock(tamutex);
      if (tasks.empty()) {
        continue;
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
  far::thread t1(consumeTask);
  far::thread t2(consumeTask);
  t.join();
  t1.join();
  t2.join();
  return 0;
}
