#ifndef __FAR_THREAD_H__
#define __FAR_THREAD_H__

#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <string>
#include <tuple>
#include <type_traits>

#include "sem.h"

namespace far {
struct this_thread {
  /**
   * @return 线程id
   */
  static uint32_t get_id() {
    if (__builtin_expect(_tid == 0, 0)) {
      _tid = syscall(SYS_gettid);
    }
    return _tid;
  }

  /**
   * @return 线程id(to string)
   */
  static const std::string &id_to_string() {
    if (__builtin_expect(_tid_str.empty() == true, 0)) {
      _tid_str = std::to_string(get_id());
    }
    return _tid_str;
  }

 private:
  static thread_local uint32_t _tid;
  static thread_local std::string _tid_str;
};

thread_local uint32_t this_thread::_tid{0};
thread_local std::string this_thread::_tid_str{""};

template <typename... Args, std::size_t... I>
inline void threadCallback(std::tuple<Args...> *pack,
                           std::index_sequence<I...>) {
  std::get<1> (*pack)(std::get<I + 2>(*pack)...);
}

template <typename Obj, typename... Args, std::size_t... I,
          typename = std::enable_if_t<std::is_class<Obj>::value>>
inline void threadCallback(Obj *obj, std::tuple<Args...> *pack,
                           std::index_sequence<I...>) {
  (obj->*std::get<2>(*pack))(std::get<I + 3>(*pack)...);
}

class thread {
 public:
  typedef pthread_t handle_t;

 public:
  thread() = default;
  thread(const thread &) = delete;
  thread &operator=(const thread &) = delete;

  thread(thread &&t) { swap(t); }

  thread &operator=(thread &&t) {
    if (&t == this) return *this;
    if (_joinable) {
      join();
    }
    swap(t);
    return *this;
  }

  template <typename Func, typename... Args,
            typename = decltype(std::declval<Func>()(std::declval<Args>()...))>
  thread(Func &&func, Args &&...args) {
    using pack_type = std::tuple<thread *, Func, Args...>;
    auto taskPack{new pack_type{this, std::forward<Func>(func),
                                std::forward<Args>(args)...}};
    struct ThreadFunc {
      static void *_run(void *arg) {
        auto _taskPack = static_cast<pack_type *>(arg);
        thread *_this = std::get<0>(*_taskPack);
        _this->_id = this_thread::get_id();
        _this->_sem.post();
        threadCallback(_taskPack, std::make_index_sequence<sizeof...(Args)>());
        delete _taskPack;
        return nullptr;
      }
    };
    int ret = pthread_create(&_handle, nullptr, ThreadFunc::_run, taskPack);
    assert(ret == 0);
    _sem.wait();
    _joinable = true;
  }

  template <typename Func, typename Obj, typename... Args,
            typename = std::enable_if_t<
                std::is_member_function_pointer<std::decay_t<Func>>::value &&
                std::is_class<std::decay_t<Obj>>::value>>
  thread(Func &&func, Obj *obj, Args &&...args) {
    using pack_type = std::tuple<thread *, Obj *, Func, Args...>;
    auto taskPack{new pack_type{this, obj, std::forward<Func>(func),
                                std::forward<Args>(args)...}};
    struct ThreadFunc {
      static void *_run(void *arg) {
        auto _taskPack = static_cast<pack_type *>(arg);
        thread *_this = std::get<0>(*_taskPack);
        _this->_id = this_thread::get_id();
        _this->_sem.post();
        threadCallback(std::get<1>(*_taskPack), _taskPack,
                       std::make_index_sequence<sizeof...(Args)>());
        delete _taskPack;
        return nullptr;
      }
    };
    int ret = pthread_create(&_handle, nullptr, ThreadFunc::_run, taskPack);
    assert(ret == 0);
    _sem.wait();
    _joinable = true;
  }

  ~thread() {
    if (_joinable) {
      std::abort();
    }
  }

  /**
   * 等待线程结束
   */
  void join() {
    int ret = pthread_join(_handle, nullptr);
    assert(ret == 0);
    _joinable = false;
    _handle = 0;
  }

  /**
   * 线程分离
   */
  void detach() {
    int ret = pthread_detach(_handle);
    assert(ret == 0);
    _joinable = false;
    _handle = 0;
  }

  bool joinable() const { return _joinable; }

  handle_t thread_handle() const { return _handle; }

  /**
   * 交换两个线程
   */
  void swap(thread &t) {
    std::swap(_joinable, t._joinable);
    std::swap(_handle, t._handle);
  }

  /**
   * 设置线程的cpu绑定
   * @param cpu_code cpu编号
   */
  void set_affinity_np(uint32_t cpu_code) {
    set_affinity_np(_handle, cpu_code);
  }

  /**
   * 设置线程的cpu绑定
   * @param handle 线程id
   * @param cpu_code cpu编号
   */
  static void set_affinity_np(handle_t handle, uint32_t cpu_code) {
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpu_code, &cpu_set);
    (void)pthread_setaffinity_np(handle, sizeof(cpu_set_t), &cpu_set);
  }

  /**
   * 获取线程绑定的cpu编号
   */
  uint32_t get_affinity_np() const { return get_affinity_np(_handle); }

  /**
   * @return 线程id
   */
  uint32_t id() const { return _id; }

  /**
   * 获取线程绑定的cpu编号
   * @param handle 线程id
   */
  static uint32_t get_affinity_np(handle_t handle) {
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    pthread_getaffinity_np(handle, sizeof(cpu_set_t), &cpu_set);
    for (int i = 0; i < CPU_SETSIZE; ++i) {
      if (CPU_ISSET(i, &cpu_set)) {
        return i;
      }
    }
    return -1;
  }

  /**
   * @return cpu核心数
   */
  static uint32_t get_core_count() { return get_nprocs_conf(); }

 private:
  bool _joinable{false};
  pthread_t _handle{0};
  uint32_t _id{0};
  sem _sem{0};
};

/**
 * 交换两个线程
 */
void swap(thread &t1, thread &t2) { t1.swap(t2); }
}  // namespace far
#endif
