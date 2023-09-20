#include <shared_mutex>
#include <unistd.h>

#include <iostream>

#include "lock.h"
#include "mutex.h"
#include "shared_mutex.h"
#include "thread.h"

yf::mutex g_mutex;
yf::mutex g_mutex1;
yf::atomic_lock g_amutex;
yf::shared_mutex g_smutex;
yf::spin_lock g_spin;

void func(int a) {
  int i = 10;
  while (i > 0) {
    --i;
    // sleep(1);
    // yf::lock_guard<yf::mutex> lock{g_mutex};
    // yf::lock_guard<yf::spin_lock> lock{g_spin};
    yf::lock_guard<yf::atomic_lock> alock{g_amutex};
    // yf::unique_lock<yf::atomic_lock> alock{g_amutex};
    // yf::unique_lock<yf::mutex> lock{g_mutex};
    // yf::unique_lock<yf::mutex> lock1{g_mutex1};
    // yf::unique_lock<yf::mutex> lock2{g_mutex};
    // lock1 = std::move(lock2);
    // lock1.swap(lock2);
    // if (!g_mutex.try_lock()) {
    //   yf::shared_lock<yf::shared_mutex> slock{g_smutex};
    //   std::cout << "trylock false: " << yf::thread::getThreadId() << " a: "
    //   << a
    //             << "\n";
    // } else {
    //   yf::shared_lock<yf::shared_mutex> slock{g_smutex};
    std::cout << "threadId: " << yf::this_thread::get_id() << " a: " << a
              << "begin\n";
    // sleep(2);
    std::cout << "threadId: " << yf::this_thread::get_id() << " a: " << a
              << "end\n";
    // g_mutex.unlock();
    // }
  }
}

void test01() {
  yf::thread t1{func, 1};
  yf::thread t2{func, 2};
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
  yf::thread t1(A::func, 1);
  t1.join();
  A a;
  yf::thread t2(&A::funcA, &a, b);
  std::cout << "b:" << b << '\n';
  t2.join();
  yf::thread t3(A{}, 1);
  t3.join();
}

void test03() {
  yf::thread t1([](int a) { std::cout << "test03 a: " << a << " lambda thread\n"; },
                1);
  yf::thread t2(std::move(t1));
  if (t1.joinable())
    t1.join();
  if (t2.joinable())
    t2.join();
}

void test04() {
  yf::thread t1(
      [](int a) {
        std::cout << "thread id: " << yf::this_thread::get_id()
                  << " test04 a: " << a << " lambda thread\n";
      },
      1);
  std::cout << "t1 threadId: " << t1.id() << std::endl;
  yf::thread t2;
  // t2 = std::move(t1);
  yf::swap(t1, t2);
  if (t1.joinable())
    t1.join();
  if (t2.joinable())
    t2.join();
}

int main() {
  std::cout << "main threadId: " << yf::this_thread::get_id() << " : "
            << yf::this_thread::get_id()
            << "\ncore count: " << yf::thread::get_core_count()
            << "\ncpu affinity: " << yf::thread::get_affinity_np(pthread_self()) << '\n';
  // while (true) {
  test01();
  // test02();
  // test03();
  // test04();
  // }
  return 0;
}
