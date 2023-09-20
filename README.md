#  yf_thread
linux系统环境下，使用c++14标准对POSIX提供的线程接口进行封装
代码实例都使用valgrind进行过内存泄漏检测
## 线程创建
实现的核心技术使用tuple实现类型擦除，从而实现类似标准库提供的thread接口
```cpp
#include <iostream>
#include "yf_thread.h"

// 1. 使用lambda创建线程
void teste01() {
    yf::thread t([](int a) {
        std::cout << "threadId: " << yf::this_thread::get_id() << " a: " << a << '\n'
    }, 1);
    if (t.joinable()) {
        t.join();
    }
}

void func(int a) {
    std::cout << "threadId: " << yf::this_thread::get_id() << " a: " << a << '\n';
}

// 2. 使用普通函数创建线程
void test02() {
    yf::thread t{func, 1};
    t.join();
}

struct A {
public:
    void operator()(int a) {
        std::cout << "A::operator() a = " << a << '\n';
    }
    void funcA(int &a) {
        std::cout << "A::funcA() a = " << a  << '\n';
        a = 3;
    }
    static void func(int a) {
        std::cout << "A::func() a = " << a << '\n';
    }
};

void test03() {
    // 3. 使用类的静态成员函数创建线程
    yf::thread t1(A::func, 1);
    t1.join();
    // 4. 使用类的成员函数创建线程
    A a;
    int count = 2;
    std::cout << "begin count:" << count << '\n';
    yf::thread t2(&A::funcA, &a, count);
    std::cout << "end count:" << count << '\n';

    // 5. 使用仿函数创建线程
    yf::thread t3(A{}, 1);
    t3.join();
}

```

## 线程同步
线程同步是保证多个线程竞争同一个资源时保证其有序竞争的一种手段，主要封装了以下几种(主要这些锁皆不可复制和移动)
1. 互斥量: mutex (底层实现为pthread_mutex_t)
2. 自旋锁: spin_lock (底层实现为pthread_spinlock_t) atomic_lock (使用atomic<bool>实现)
3. 共享锁: shared_lock (底层实现为pthread_rwlock_t)
4. 信号量: sem (底层实现为sem_t)
5. 条件变量: conditional_variable (底层实现为pthread_cond_t)
```cpp
#include <iostream>

#include "yf_lock.h"
#include "yf_mutex.h"
#include "yf_thread.h"

yf::mutex g_mutex;
yf::mutex g_mutex1;

void func() {
    while (true) {
        sleep(1);
        yf::lock_guard<yf::mutex> lock{g_mutex};
        // yf::unique_lock<yf::mutex> lock{g_mutex};
        // yf::unique_lock<yf::mutex> lock1{g_mutex1};
        // yf::unique_lock<yf::mutex> lock2{g_mutex};
        // lock1 = std::move(lock2);
        // lock1.swap(lock2);
        std::cout << "threadId: " << yf::this_thread::get_id() << " a: " << a << "begin\n";
        sleep(2);
        std::cout << "threadId: " << yf::this_thread::get_id()() << " a: " << a << "end\n";
    }
}

void test01() {
  yf::thread t1{func, 1};
  yf::thread t2{func, 2};
  t1.join();
  t2.join();
}

int main(int argc, char *argv[]) {
    test01();
    return 0;
}

```

除了这些还提供了类似stl提供的lock_guard/unique_lock/shared_lock template class(RAII机制)以此用于管理各类锁。

## jthread
```cpp
#include <iostream>

#include "jthread.h"

void test() { std::cout << yf::this_thread::get_id() << " test()\n"; }

void test01(int num, std::string &name) {
    std::cout << yf::this_thread::get_id() << " test01()\n";
    std::cout << "num: " << num << "name: " << name << std::endl;
    name = "bbb";
}

int main() {
  std::cout << yf::this_thread::get_id() << " main()\n";
  // yf::jthread jt{test};
  // yf::jthread jt1{[]() { std::cout << yf::thread::get_id() << " lamdba()\n";
  // }};
  std::string name = "aaa";
  {
    yf::jthread jt2{test01, 12, name};
    std::cout << "t2 thread id: " << jt2.id() << std::endl;
  }
  std::cout << name << std::endl;
  return 0;
}
```

## 生成测试文件方法
```
语言标准: c++14
cmake版本: 3.5或以上皆可

1. 在CMakeLists.txt同级文件下创建build、libs、bin目录
2. cd build
3. cmake .. (如果想生成测试文件请使用cmake -DUSE_TEST ..)
4. cmake --build .

build: 编译目录存放一堆中间文件
libs: 存放生成的动态库和静态库
test: 存放测试文件
bin: 用于存放生成的可执行文件一般是测试的时候才会用到
```
