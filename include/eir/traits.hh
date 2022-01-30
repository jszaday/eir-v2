#ifndef EIR_TRAITS_HH
#define EIR_TRAITS_HH

#include <type_traits>

namespace eir {
template <class T, std::size_t = sizeof(T)>
std::true_type is_complete_impl(T *);

std::false_type is_complete_impl(...);

template <class T>
using is_complete = decltype(is_complete_impl(std::declval<T *>()));

template <class T> constexpr bool is_complete_v = is_complete<T>::value;
} // namespace eir

#endif
