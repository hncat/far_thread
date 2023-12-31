#ifndef __FAR_LOCK_HH__
#define __FAR_LOCK_HH__

#include <type_traits>

#include "mutex.hh"
#include "shared_mutex.hh"

namespace far {
#define assert_unlock(__mutex, __owns) assert(__mutex &&__owns)
#define assert_lock(__mutex, __owns) assert_lock(__mutex, !__owns)

template <typename _Mutex>
class lock_guard {
 public:
  typedef _Mutex mutex_type;
  typedef _Mutex &mutex_reference;
  typedef _Mutex *mutex_pointer;

 public:
  explicit lock_guard(mutex_reference __m) : _mutex(__m) { _mutex.lock(); }
  lock_guard(mutex_reference &__m, adopt_lock_t) : _mutex(__m) {}
  ~lock_guard() { _mutex.unlock(); }
  lock_guard(const lock_guard &) = delete;
  lock_guard &operator=(const lock_guard &) = delete;
  inline mutex_reference get() { return _mutex; }

 private:
  mutex_reference _mutex;
};

template <typename _Mutex>
class unique_lock {
 public:
  typedef _Mutex mutex_type;
  typedef _Mutex &mutex_reference;
  typedef _Mutex *mutex_pointer;

 public:
  unique_lock() : _mutex(nullptr), _owns(false) {}
  explicit unique_lock(mutex_reference __m)
      : _mutex(std::addressof(__m)), _owns(false) {
    _mutex->lock();
    _owns = true;
  }
  unique_lock(mutex_reference __m, defer_lock_t)
      : _mutex(std::addressof(__m)), _owns(false) {}
  unique_lock(mutex_reference __m, adopt_lock_t)
      : _mutex(std::addressof(__m)), _owns(true) {}
  unique_lock(mutex_reference __m, try_lock_t)
      : _mutex(std::addressof(__m)), _owns(_mutex->try_lock()) {}
  unique_lock(unique_lock &&__lock) : _mutex(__lock._mutex), _owns(__lock._owns) {
    __lock._mutex = nullptr;
    __lock._owns = false;
  }
  unique_lock &operator=(unique_lock &&__lock) {
    if (this == std::addressof(__lock)) {
      return *this;
    }
    if (_owns) {
      _mutex->unlock();
    }
    _mutex = nullptr;
    _owns = false;
    __lock.swap(*this);
    return *this;
  }
  inline void lock() {
    assert_lock(_mutex, _owns);
    _mutex->lock();
    _owns = true;
  }
  inline bool try_lock() {
    assert_lock(_mutex, _owns);
    _owns = _mutex->try_lock();
    return _owns;
  }
  inline void unlock() {
    assert_unlock(_mutex, _owns);
    _mutex->unlock();
    _owns = false;
  }
  inline void swap(unique_lock &__lock) {
    std::swap(_mutex, __lock._mutex);
    std::swap(_owns, __lock._owns);
  }
  inline bool owns_lock() const { return _owns; }
  inline mutex_pointer get() { return _mutex; }
  inline mutex_pointer release() {
    mutex_pointer __ret = _mutex;
    _mutex = nullptr;
    _owns = false;
    return __ret;
  }
  ~unique_lock() {
    if (_mutex) {
      _mutex->unlock();
    }
  }
  unique_lock(const unique_lock &) = delete;
  unique_lock &operator=(const unique_lock &) = delete;

 private:
  mutex_pointer _mutex;
  bool _owns;
};

template <typename _Mutex>
inline void swap(unique_lock<_Mutex> &__x, unique_lock<_Mutex> &__y) {
  __x.swap(__y);
}

template <typename _Mutex>
class shared_lock {
 public:
  typedef _Mutex mutex_type;
  typedef _Mutex &mutex_reference;
  typedef _Mutex *mutex_pointer;

 public:
  shared_lock() : _mutex(nullptr), _owns(false) {}
  explicit shared_lock(mutex_reference __m)
      : _mutex(std::addressof(__m)), _owns(false) {
    _mutex->lock_shared();
    _owns = true;
  }
  shared_lock(mutex_reference __m, defer_lock_t)
      : _mutex(std::addressof(__m)), _owns(false) {}
  shared_lock(mutex_reference __m, adopt_lock_t)
      : _mutex(std::addressof(__m)), _owns(true) {}
  shared_lock(mutex_reference __m, try_lock_t)
      : _mutex(std::addressof(__m)), _owns(_mutex->try_lock_shared()) {}
  shared_lock(shared_lock &&__lock) : _mutex(__lock._mutex), _owns(__lock._owns) {
    __lock._mutex = nullptr;
    __lock._owns = false;
  }
  shared_lock &operator=(shared_lock &&__lock) {
    if (this == std::addressof(__lock)) {
      return *this;
    }
    if (_owns) {
      _mutex->unlock_shared();
    }
    _mutex = nullptr;
    _owns = false;
    __lock.swap(*this);
  }
  void lock_shared() {
    assert_lock(_mutex, _owns);
    _mutex->lock_shared();
  }
  bool try_lock_shared() {
    assert_lock(_mutex, _owns);
    _owns = _mutex->try_lock_shared();
    return _owns;
  }
  void unlock_shared() {
    assert_unlock(_mutex, _owns);
    _mutex->unlock_shared();
    _owns = false;
  }
  void swap(shared_lock &__lock) {
    std::swap(_mutex, __lock._mutex);
    std::swap(_owns, __lock._owns);
  }
  bool owns_lock() const { return _owns; }
  mutex_pointer get() { return _mutex; }
  mutex_pointer release() {
    mutex_pointer __ret = _mutex;
    _mutex = nullptr;
    _owns = false;
    return __ret;
  }
  ~shared_lock() {
    if (_owns) {
      _mutex->unlock_shared();
    }
  }
  shared_lock(const shared_lock &) = delete;
  shared_lock &operator=(const shared_lock &) = delete;

 private:
  mutex_pointer _mutex;
  bool _owns;
};

template <typename _Mutex>
void swap(shared_lock<_Mutex> &__x, shared_lock<_Mutex> &__y) {
  __x.swap(__y);
}
}  // namespace far

#endif
