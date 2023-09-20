#ifndef __YF_MUTEX_H__
#define __YF_MUTEX_H__

#include <pthread.h>

#include <atomic>
#include <type_traits>

namespace yf {
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

class mutex_basic {
 protected:
  mutex_basic() = default;
  ~mutex_basic() = default;

 public:
  mutex_basic(const mutex_basic &) = delete;
  mutex_basic &operator=(const mutex_basic &) = delete;
  mutex_basic(mutex_basic &&) = delete;
  mutex_basic &operator=(mutex_basic &&) = delete;
};

class mutex : private mutex_basic {
  friend class conditional_variable;

 public:
  mutex();
  ~mutex();
  void lock();
  bool try_lock();
  void unlock();

 private:
  pthread_mutex_t _mutex;
};

class atomic_lock : public mutex_basic {
 public:
  atomic_lock();
  ~atomic_lock() = default;
  void lock();
  bool try_lock();
  void unlock();

 private:
  std::atomic<bool> _mutex;
};

class spin_lock : public mutex_basic {
 public:
  explicit spin_lock(bool process_shared = false);
  ~spin_lock();
  void lock();
  bool try_lock();
  void unlock();

 private:
  pthread_spinlock_t _spin;
};
}  // namespace yf
#endif
