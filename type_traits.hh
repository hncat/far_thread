#ifndef __FAR_TYPE_TRAITS__
#define __FAR_TYPE_TRAITS__

#include <type_traits>

namespace far {
template <typename Func, typename... Args>
struct is_result_void {
  constexpr static bool value = std::is_void<decltype(std::declval<Func>()(
      std::declval<Args>()...))>::value;
};
}  // namespace far

#endif
