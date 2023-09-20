#ifndef __YF_TYPE_TRAITS__
#define __YF_TYPE_TRAITS__

#include <type_traits>

namespace yf {
template <typename Func, typename... Args>
struct is_result_void {
  constexpr static bool value = std::is_void<decltype(std::declval<Func>()(
      std::declval<Args>()...))>::value;
};
}  // namespace yf

#endif
