#ifndef EIR_PROPERTY_HH
#define EIR_PROPERTY_HH

#include <eir/serializable.hh>
#include <experimental/type_traits>
#include <type_traits>
#include <utility>

#define EIR_PROPERTY_ACCESSOR(class, name)                                     \
  const auto &name(void) const { return this->name##_; }                       \
  auto name(void) { return eir::property_proxy_(this, &(this->name##_)); }     \
  void EIR_CONVERT_PROPERTY(name)(eir::converter & c) {                        \
    c | std::tie(#name, this->name##_);                                        \
  }                                                                            \
  inline static auto name##_registrar_ = eir::field_registrar_<class>(         \
      #class, #name, &class ::EIR_CONVERT_PROPERTY(name));

#define EIR_CLASS_STUB(class)                                                  \
  inline static auto class##_registrar_ = eir::stub_registrar_<class>(#class);

namespace eir {

template <class T>
using on_assign_t = decltype(std::declval<T &>().on_assign());

template <typename T>
using supports_on_assign = std::experimental::is_detected<on_assign_t, T>;

template <typename Class, typename T> class property_proxy_ {
  Class *base_;
  T *impl_;

  using self_type = property_proxy_<Class, T>;

public:
  property_proxy_(Class *base, T *property = nullptr)
      : base_(base), impl_(property) {}

  const T &operator*(void) const { return *(this->impl_); }

  operator const T &(void) const { return **this; }

  template <typename _ = T>
  std::enable_if_t<std::is_move_assignable_v<_>, self_type &> operator=(T &&t) {
    return this->apply([](T &a, T &&b) { a = std::move(b); }, std::move(t));
  }

  template <typename _ = T>
  std::enable_if_t<std::is_copy_assignable_v<_>, self_type &>
  operator=(const T &t) {
    return this->apply([](T &a, const T &b) { a = b; }, std::move(t));
  }

  template <typename Fn, typename... Args>
  self_type &apply(const Fn &fn, Args &&...args) {
    if constexpr (supports_on_assign<Class>::value) {
      this->base_->on_assign();
    }

    fn(*(this->impl_), std::forward<Args>(args)...);

    return *this;
  }
};
} // namespace eir

#endif
