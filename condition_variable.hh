#ifndef __FAR_COND_VAR_HH__
#define __FAR_COND_VAR_HH__

#include "lock.hh"
#include "type_traits.hh"

namespace far {
class __condvar {
 public:
  __condvar() = default;
  ~__condvar() {
    int __e = pthread_cond_destroy(&_cond);
    assert(__e == 0);
  }
  __condvar(const __condvar &) = delete;
  __condvar &operator=(const __condvar &) = delete;

  /**
   * @brief 等待唤醒
   * @param __m 关联的mutex
   */
  inline void wait(mutex &__m) {
    int __e = pthread_cond_wait(&_cond, &__m._mutex);
    assert(__e == 0);
  }

  /**
   * @brief 唤醒一个线程
   */
  inline void notfiy_one() {
    int __e = pthread_cond_signal(&_cond);
    assert(__e == 0);
  }

  /**
   * @brief 唤醒所有线程
   */
  inline void notfiy_all() {
    int __e = pthread_cond_broadcast(&_cond);
    assert(__e == 0);
  }

 private:
  pthread_cond_t _cond = PTHREAD_COND_INITIALIZER;
};

class conditional_variable {
 public:
  conditional_variable() = default;
  ~conditional_variable() = default;
  conditional_variable(const conditional_variable &) = delete;
  conditional_variable &operator=(const conditional_variable &) = delete;
  /**
   * @brief 等待线程唤醒
   * @param lock 对应unique_lock<mutex>
   */
  void wait(unique_lock<mutex> &__lock) { _cond_var.wait(*__lock.get()); }
  /**
   * @brief 等待线程唤醒
   * @param lock 对应unique_lock<mutex>
   * @param func 对应条件函数(条件为true时退出循环等待)
   */
  template <typename Func, typename... Args,
            typename = typename std::enable_if<
                !is_result_void<Func, Args...>::value>::type>
  void wait(unique_lock<mutex> &__lock, Func &&__func, Args &&...__args) {
    while (!__func(std::forward<Args>(__args)...)) {
      wait(__lock);
    }
  }
  /**
   * @brief 唤醒一个线程
   */
  inline void notfiy_one() { _cond_var.notfiy_one(); }
  /**
   * @brief 唤醒所有线程
   */
  void notify_all() { _cond_var.notfiy_all(); }

 private:
  __condvar _cond_var;
};
}  // namespace far

#endif
