#ifndef __FAR_JTHREAD_H__
#define __FAR_JTHREAD_H__
#include "thread.h"

namespace far {
class jthread {
 public:
  jthread() = default;

  template <typename... Args>
  jthread(Args &&...args) : _t{thread{std::forward<Args>(args)...}} {}

  jthread(const jthread &) = delete;
  jthread &operator=(const jthread &) = delete;

  jthread(jthread &&jt) : _t{std::move(jt._t)} {}

  jthread &operator=(jthread &&jt) {
    if (&jt == this) return *this;
    jt.swap(*this);
    return *this;
  }

  jthread(thread t) : _t{std::move(t)} {}

  jthread(thread &&t) : _t{std::move(t)} {}

  ~jthread() {
    if (joinable()) {
      join();
    }
  }

  inline void join() { _t.join(); }

  inline void detach() { _t.detach(); }

  inline bool joinable() { return _t.joinable(); }

  inline thread::handle_t thread_handle() const { return _t.thread_handle(); }

  inline void swap(jthread &jt) { jt._t.swap(_t); }

  inline void set_affinity_np(uint32_t cpu_code) {
    _t.set_affinity_np(cpu_code);
  }

  inline uint32_t get_affinity_np() const { return _t.get_affinity_np(); }

  inline uint32_t id() const { return _t.id(); }

 private:
  thread _t;
};
inline void swap(jthread &jt1, jthread &jt2) { jt1.swap(jt2); }
}  // namespace far
#endif
