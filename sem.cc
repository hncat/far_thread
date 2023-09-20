#include "sem.h"

namespace yf {
sem::sem(int value, bool shared) { sem_init(&_sem, shared ? 1 : 0, value); }

sem::~sem() { sem_destroy(&_sem); }

bool sem::post() { return sem_post(&_sem) == 0; }

bool sem::wait() { return sem_wait(&_sem) == 0; }
}  // namespace yf