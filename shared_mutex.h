#ifndef __FAR_SHARED_MUTEX_H__
#define __FAR_SHARED_MUTEX_H__

#include <pthread.h>

namespace far {
class shared_mutex {
 public:
  shared_mutex();
  ~shared_mutex();
  void lock();
  bool try_lock();
  void unlock();
  void lock_shared();
  bool try_lock_shared();
  void unlock_shared();
  shared_mutex(const shared_mutex &) = delete;
  shared_mutex &operator=(const shared_mutex &) = delete;

 private:
  pthread_rwlock_t _mutex;
};
}  // namespace far

#endif
