#include "shared_mutex.h"

#include <assert.h>
#include <errno.h>

namespace far {
shared_mutex::shared_mutex() : _mutex(PTHREAD_RWLOCK_INITIALIZER) {}

shared_mutex::~shared_mutex() { pthread_rwlock_destroy(&_mutex); }

void shared_mutex::lock() {
  int ret = pthread_rwlock_wrlock(&_mutex);
  if (ret == EDEADLK || ret == EINVAL) {
    assert(false);
  }
}

bool shared_mutex::try_lock() {
  int ret = pthread_rwlock_trywrlock(&_mutex) == 0;
  if (ret == EBUSY) return false;
  assert(ret == 0);
  return true;
}

void shared_mutex::unlock() {
  int ret = pthread_rwlock_unlock(&_mutex);
  assert(ret == 0);
}

void shared_mutex::lock_shared() {
  int ret;
  do ret = pthread_rwlock_rdlock(&_mutex);
  while (ret == EAGAIN);
  if (ret == EDEADLK) assert(false);
}

bool shared_mutex::try_lock_shared() {
  int ret = pthread_rwlock_tryrdlock(&_mutex) == 0;
  if (ret == EBUSY || ret == EAGAIN) return false;
  assert(ret == 0);
  return true;
}

void shared_mutex::unlock_shared() { unlock(); }
}  // namespace far
