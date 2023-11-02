#ifndef __FAR_SEM_H__
#define __FAR_SEM_H__

#include <semaphore.h>

namespace far {
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
}  // namespace far

#endif
