#ifndef __FAR_FUTURE_HH__
#define __FAR_FUTURE_HH__

#include <future>

#include "condition_variable.hh"
#include "mutex.hh"

namespace far {
template <typename T>
class future {
 public:
  future() = default;


 private:
  conditional_variable _cond_var;
  mutex _mutex;
};
}  // namespace far

#endif