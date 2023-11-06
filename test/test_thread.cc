#include <unistd.h>

#include <iostream>
#include <shared_mutex>

#include "lock.hh"
#include "mutex.hh"
#include "shared_mutex.hh"
#include "thread.hh"

far::mutex g_mutex;
far::mutex g_mutex1;
far::atomic_lock g_amutex;
far::shared_mutex g_smutex;
far::spin_lock g_spin;

void func(int a) {
  int i = 3;
  while (i > 0) {
    --i;
    // sleep(1);
    // far::lock_guard<far::mutex> lock{g_mutex};
    // far::lock_guard<far::spin_lock> lock{g_spin};
    // far::lock_guard<far::atomic_lock> alock{g_amutex};
    // far::unique_lock<far::atomic_lock> alock{g_amutex};
    // far::unique_lock<far::mutex> lock{g_mutex};
    // far::unique_lock<far::mutex> lock1{g_mutex1};
    // far::unique_lock<far::mutex> lock2{g_mutex};
    // lock1 = std::move(lock2);
    // lock1.swap(lock2);
    if (!g_mutex.try_lock()) {
      far::shared_lock<far::shared_mutex> slock{g_smutex};
      sleep(1);
      std::cout << "trylock false: " << far::this_thread::get_id()
                << " a: " << a << "\n";
    } else {
      // far::shared_lock<far::shared_mutex> slock{g_smutex};
      //   far::unique_lock<far::shared_mutex> slock{g_smutex};
      // std::cout << "shared_mutex threadId: " << far::this_thread::get_id() <<
      // " a: " << a
      //           << "begin\n";
      std::cout << "lock success threadId: " << far::this_thread::get_id()
                << " a: " << a << "end\n";
      g_mutex.unlock();
      far::this_thread::yield();
    }
  }
}

void test01() {
  far::thread t1{func, 1};
  far::thread t2{func, 2};
  t1.swap(t2);
  t1.join();
  t2.join();
}

struct A {
 public:
  void operator()(int a) { std::cout << a << "A::operator()\n"; }
  void funcA(int a) { std::cout << a << "funcA()\n"; }
  static void func(int a) { std::cout << a << "A::func()\n"; }
};

void test02() {
  int b = 10;
  far::thread t1(A::func, 1);
  t1.join();
  A a;
  far::thread t2(&A::funcA, &a, b);
  std::cout << "b:" << b << '\n';
  t2.join();
  far::thread t3(A{}, 1);
  t3.join();
}

void test03() {
  far::thread t1(
      [](int a) { std::cout << "test03 a: " << a << " lambda thread\n"; }, 1);
  far::thread t2(std::move(t1));
  if (t1.joinable()) t1.join();
  if (t2.joinable()) t2.join();
}

void test04() {
  far::thread t1(
      [](int a) {
        std::cout << "thread id: " << far::this_thread::get_id()
                  << " test04 a: " << a << " lambda thread\n";
      },
      1);
  std::cout << "t1 threadId: " << t1.id() << std::endl;
  far::thread t2;
  // t2 = std::move(t1);
  far::swap(t1, t2);
  if (t1.joinable()) t1.join();
  if (t2.joinable()) t2.join();
}

int main() {
  std::cout << "main threadId: " << far::this_thread::get_id() << " : "
            << far::this_thread::get_id()
            << "\ncore count: " << far::thread::get_core_count()
            << "\ncpu affinity: "
            << far::thread::get_affinity_np(pthread_self()) << '\n';
  // while (true) {
  test01();
  // test02();
  // test03();
  // test04();
  // }
  return 0;
}
