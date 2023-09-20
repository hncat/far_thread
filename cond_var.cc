#include "cond_var.h"

namespace yf {
conditional_variable::conditional_variable()
    : _cond(PTHREAD_COND_INITIALIZER) {}

conditional_variable::~conditional_variable() { pthread_cond_destroy(&_cond); }

void conditional_variable::wait(lock_guard<mutex> &lock) {
  pthread_cond_wait(&_cond, &lock.get()._mutex);
}

void conditional_variable::notfiy_one() { pthread_cond_signal(&_cond); }

void conditional_variable::notify_all() { pthread_cond_broadcast(&_cond); }
}  // namespace yf
