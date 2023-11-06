#ifndef __FAR_JTHREAD_HH__
#define __FAR_JTHREAD_HH__
#include "thread.hh"

namespace far {
class jthread {
 public:
  jthread() = default;

  template <typename... _Args>
  jthread(_Args &&...__args) : _t{thread{std::forward<_Args>(__args)...}} {}

  jthread(const jthread &) = delete;
  jthread &operator=(const jthread &) = delete;

  jthread(jthread &&__jt) : _t{std::move(__jt._t)} {}

  jthread &operator=(jthread &&__jt) {
    if (&__jt == this) return *this;
    __jt.swap(*this);
    return *this;
  }

  jthread(thread __t) : _t{std::move(__t)} {}

  jthread(thread &&__t) : _t{std::move(__t)} {}

  ~jthread() {
    if (joinable()) {
      join();
    }
  }

  inline void join() { _t.join(); }

  inline void detach() { _t.detach(); }

  inline bool joinable() { return _t.joinable(); }

  inline thread::handle_t thread_handle() const { return _t.thread_handle(); }

  inline void swap(jthread &__jt) { __jt._t.swap(_t); }

  inline void set_affinity_np(uint32_t __cpu_code) {
    _t.set_affinity_np(__cpu_code);
  }

  inline uint32_t get_affinity_np() const { return _t.get_affinity_np(); }

  inline uint32_t id() const { return _t.id(); }

 private:
  thread _t;
};
inline void swap(jthread &__jt1, jthread &__jt2) { __jt1.swap(__jt2); }
}  // namespace far
#endif
