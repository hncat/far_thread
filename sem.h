#ifndef __YF_SEM_H__
#define __YF_SEM_H__

#include <semaphore.h>

namespace yf {
class sem {
 public:
  explicit sem(int value, bool shared = false);
  ~sem();
  sem(const sem &) = delete;
  sem &operator=(const sem &) = delete;
  bool post();
  bool wait();

 private:
  sem_t _sem;
};
}  // namespace yf

#endif
