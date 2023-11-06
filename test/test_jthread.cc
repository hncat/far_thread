#include <iostream>
#include <future>

#include "jthread.hh"

void test() { std::cout << far::this_thread::id_to_string() << " test()\n"; }

void test01(int num, std::string &name) {
    std::cout << far::this_thread::get_id() << " test01()\n";
    std::cout << "num: " << num << "name: " << name << std::endl;
    name = "bbb";
}

int main() {
  std::future<int> fa;
  std::cout << far::this_thread::get_id() << " main()\n";
  for (int i = 0; i < 100; ++i) {
    far::jthread jt{test};
  }
  // far::jthread jt1{[]() { std::cout << far::thread::get_id() << " lamdba()\n";
  // }};
  std::string name = "aaa";
  {
    far::jthread jt2{test01, 12, name};
    std::cout << "t2 thread id: " << jt2.id() << std::endl;
  }
  std::cout << name << std::endl;
  return 0;
}
