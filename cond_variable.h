#ifndef __FAR_COND_VAR_H__
#define __FAR_COND_VAR_H__

#include "lock.h"
#include "type_traits.h"

namespace far {
class conditional_variable {
 public:
  conditional_variable();
  ~conditional_variable();
  void wait(lock_guard<mutex> &lock);
  template <typename Func, typename... Args,
            typename = typename std::enable_if<
                !is_result_void<Func, Args...>::value>::type>
  void wait_for(lock_guard<mutex> &lock, Func &&func, Args &&...args) {
    while (func(std::forward<Args>(args)...)) {
      wait(lock);
    }
  }
  void notfiy_one();
  void notify_all();

 private:
  pthread_cond_t _cond;
};
}  // namespace far

#endif
