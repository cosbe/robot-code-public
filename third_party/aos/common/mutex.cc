#include "third_party/aos/common/mutex.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "third_party/aos/common/die.h"
#include "third_party/aos/common/type_traits.h"

namespace aos {

Mutex::Mutex() : impl_() {}

Mutex::~Mutex() {
  if (__builtin_expect(mutex_islocked(&impl_), false)) {
    ::aos::Die("destroying locked mutex %p (aka %p)\n", this, &impl_);
  }
}

// Lock and Unlock use the return values of mutex_lock/mutex_unlock
// to determine whether the lock/unlock succeeded.

bool Mutex::Lock() {
  const int ret = mutex_grab(&impl_);
  if (ret == 0) {
    return false;
  } else if (ret == 1) {
    return true;
  } else {
    ::aos::Die("mutex_grab(%p(=%" PRIu32 ")) failed with %d\n", &impl_,
               impl_.futex, ret);
  }
}

void Mutex::Unlock() {
  mutex_unlock(&impl_);
}

Mutex::State Mutex::TryLock() {
  const int ret = mutex_trylock(&impl_);
  switch (ret) {
    case 0:
      return State::kLocked;
    case 1:
      return State::kOwnerDied;
    case 4:
      return State::kLockFailed;
    default:
      ::aos::Die("mutex_trylock(%p(=%" PRIu32 ")) failed with %d\n", &impl_,
                 impl_.futex, ret);
  }
}

bool Mutex::OwnedBySelf() const {
  return mutex_islocked(&impl_);
}

}  // namespace aos
