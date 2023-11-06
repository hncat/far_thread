#ifndef __FAR_SEM_HH__
#define __FAR_SEM_HH__

#include <semaphore.h>
#include <assert.h>
#include <errno.h>

namespace far {
class sem {
 public:
  explicit sem(int value, bool shared = false) {
    int __e = sem_init(&_sem, shared ? 1 : 0, value);
    assert(__e == 0);
  }
  ~sem() {
    int __e = sem_destroy(&_sem);
    assert(__e == 0);
  }
  sem(const sem &) = delete;
  sem &operator=(const sem &) = delete;
  /**
   * @brief 信号量+1
   */
  inline void post() {
    int __e = sem_post(&_sem);
    assert(__e == 0);
  }
  /**
   * @brief 信号量-1
   */
  inline void wait() {
    int __e = sem_wait(&_sem);
    assert(__e == 0);
  }

 private:
  sem_t _sem;
};
}  // namespace far

#endif
