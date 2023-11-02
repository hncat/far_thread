#include "mutex.h"

#include <assert.h>
#include <errno.h>

namespace far {
mutex::mutex() : _mutex(PTHREAD_MUTEX_INITIALIZER) {}

mutex::~mutex() { pthread_mutex_destroy(&_mutex); }

void mutex::lock() {
  int err = pthread_mutex_lock(&_mutex);
  assert(err == 0);
}

bool mutex::try_lock() { return pthread_mutex_trylock(&_mutex); }

void mutex::unlock() { pthread_mutex_unlock(&_mutex); }

atomic_lock::atomic_lock() : _mutex{true} {}

void atomic_lock::lock() {
  bool islock = true;
  while (
      !_mutex.compare_exchange_weak(islock, false, std::memory_order_relaxed)) {
    islock = true;
  }
}

bool atomic_lock::try_lock() {
  bool islock = true;
  return _mutex.compare_exchange_weak(islock, false, std::memory_order_relaxed);
}

void atomic_lock::unlock() { _mutex.store(true, std::memory_order_relaxed); }

spin_lock::spin_lock(bool process_shared) {
  int err;
  if (process_shared) {
    err = pthread_spin_init(&_spin, PTHREAD_PROCESS_SHARED);
  } else {
    err = pthread_spin_init(&_spin, PTHREAD_PROCESS_PRIVATE);
  }
  assert(err == 0 || err == EAGAIN);
}

spin_lock::~spin_lock() { pthread_spin_destroy(&_spin); }

void spin_lock::lock() {
  int err = pthread_spin_lock(&_spin);
  assert(err != EDEADLK);
}

bool spin_lock::try_lock() {
  int err = pthread_spin_trylock(&_spin);
  if (err == EBUSY) {
    return false;
  }
  assert(err == 0);
  return true;
}

void spin_lock::unlock() { pthread_spin_unlock(&_spin); }
}  // namespace far
