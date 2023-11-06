#ifndef __FAR_SHARED_MUTEX_HH__
#define __FAR_SHARED_MUTEX_HH__

#include <pthread.h>
#include <assert.h>
#include <errno.h>

namespace far {
class shared_mutex {
 public:
  shared_mutex() = default;
  ~shared_mutex() {
    int __e = pthread_rwlock_destroy(&_mutex);
    assert(__e == 0);
  }
  /**
   * @brief 独占锁
   */
  inline void lock() {
    int __e = pthread_rwlock_wrlock(&_mutex);
    if (__e == EDEADLK || __e == EINVAL) {
      assert(false);
    }
  }
  /**
   * @brief 尝试性独占锁
   * @returns 独占锁是否成功
   */
  inline bool try_lock() {
    int __e = pthread_rwlock_trywrlock(&_mutex);
    if (__e == EBUSY) return false;
    assert(__e == 0);
    return true;
  }
  /**
   * @brief 解锁独占锁
   */
  inline void unlock() {
    int __e = pthread_rwlock_unlock(&_mutex);
    assert(__e == 0);
  }
  /**
   * @brief 共享锁
   */
  inline void lock_shared() {
    int ret;
    do ret = pthread_rwlock_rdlock(&_mutex);
    while (ret == EAGAIN);
    if (ret == EDEADLK) assert(false);
  }
  /**
   * @brief 尝试性共享锁
   * @returns 共享锁是否成功
   */
  inline bool try_lock_shared() {
    int __e = pthread_rwlock_tryrdlock(&_mutex) == 0;
    if (__e == EBUSY || __e == EAGAIN) return false;
    assert(__e == 0);
    return true;
  }
  /**
   * @brief 解锁共享锁
   */
  inline void unlock_shared() { unlock(); }
  shared_mutex(const shared_mutex &) = delete;
  shared_mutex &operator=(const shared_mutex &) = delete;

 private:
  pthread_rwlock_t _mutex = PTHREAD_RWLOCK_INITIALIZER;
};
}  // namespace far

#endif
