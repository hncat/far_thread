#ifndef __FAR_MUTEX_HH__
#define __FAR_MUTEX_HH__

#include <pthread.h>

#include <atomic>
#include <type_traits>
#include <assert.h>
#include <errno.h>

namespace far {
// mutex已加锁不加锁策略
struct adopt_lock_t {
  explicit adopt_lock_t() = default;
};
// mutex未加锁不加锁策略
struct defer_lock_t {
  explicit defer_lock_t() = default;
};
// 使用try_lock策略
struct try_lock_t {
  explicit try_lock_t() = default;
};

// 策略相关
constexpr adopt_lock_t adopt_lock{};
constexpr defer_lock_t defer_lock{};
constexpr try_lock_t try_lock{};

class mutex {
  friend class __condvar;

 public:
  mutex() = default;
  ~mutex() {
    int __e = pthread_mutex_destroy(&_mutex);
    assert(__e == 0);
  }
  mutex(const mutex &) = delete;
  mutex &operator=(const mutex &) = delete;
  /**
   * @brief 加锁
   */
  inline void lock() {
    int __e = pthread_mutex_lock(&_mutex);
    assert(__e == 0);
  }
  /**
   * @brief 尝试性加锁
   * @returns 加锁是否成功
   */
  inline bool try_lock() {
    int __e = pthread_mutex_trylock(&_mutex);
    if (__e == EBUSY) return false;
    assert(__e == 0);
    return true;
  }
  /**
   * @brief 解锁
   */
  inline void unlock() {
    int __e = pthread_mutex_unlock(&_mutex);
    assert(__e == 0);
  }

 private:
  pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
};

class atomic_lock {
 public:
  atomic_lock() = default;
  ~atomic_lock() = default;
  atomic_lock(const atomic_lock &) = delete;
  atomic_lock &operator=(const atomic_lock &) = delete;
  /**
   * @brief 加锁
   */
  inline void lock() {
    bool islock = true;
    while (!_mutex.compare_exchange_weak(islock, false,
                                         std::memory_order_relaxed)) {
      islock = true;
    }
  }
  /**
   * @brief 尝试性加锁
   * @returns 是否加锁成功
   */
  inline bool try_lock() {
    bool islock = true;
    return _mutex.compare_exchange_weak(islock, false,
                                        std::memory_order_relaxed);
  }
  /**
   * @brief 解锁
   */
  inline void unlock() { _mutex.store(true, std::memory_order_relaxed); }

 private:
  std::atomic<bool> _mutex{true};
};

class spin_lock {
 public:
  explicit spin_lock(bool __process_shared = false) {
    int err;
    if (__process_shared) {
      err = pthread_spin_init(&_spin, PTHREAD_PROCESS_SHARED);
    } else {
      err = pthread_spin_init(&_spin, PTHREAD_PROCESS_PRIVATE);
    }
    assert(err == 0 || err == EAGAIN);
  }
  ~spin_lock() {
    int __e = pthread_spin_destroy(&_spin);
    assert(__e == 0);
  }
  /**
   * @brief 加锁
   */
  inline void lock() {
    int __e = pthread_spin_lock(&_spin);
    assert(__e == 0);
  }
  /**
   * @brief 尝试性加锁
   * @returns 加锁是否成功
   */
  inline bool try_lock() {
    int __e = pthread_spin_trylock(&_spin);
    if (__e == EBUSY) {
      return false;
    }
    assert(__e == 0);
    return true;
  }
  /**
   * @brief 解锁
   */
  inline void unlock() {
    int __e = pthread_spin_unlock(&_spin);
    assert(__e == 0);
  }

 private:
  pthread_spinlock_t _spin;
};
}  // namespace far
#endif
