#ifndef __FAR_FUTURE_HH__
#define __FAR_FUTURE_HH__

#include "condition_variable.hh"

namespace far {
class future {
 public:
 private:
  conditional_variable _cond_var;
};
}  // namespace far

#endif