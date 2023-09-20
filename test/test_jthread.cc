#include <iostream>

#include "jthread.h"

void test() { std::cout << yf::this_thread::id_to_string() << " test()\n"; }

void test01(int num, std::string &name) {
    std::cout << yf::this_thread::get_id() << " test01()\n";
    std::cout << "num: " << num << "name: " << name << std::endl;
    name = "bbb";
}

int main() {
  std::cout << yf::this_thread::get_id() << " main()\n";
  for (int i = 0; i < 100; ++i) {
    yf::jthread jt{test};
  }
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
