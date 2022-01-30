#ifndef EIR_SERIALIZABLE_HH
#define EIR_SERIALIZABLE_HH

#include <eir/converter.hh>
#include <eir/singleton.hh>
#include <eir/traits.hh>
#include <map>
#include <string>
#include <typeindex>

namespace eir {
template <typename T>
using field_map = std::map<std::string, converter_fn_t<T>>;

class registered_fields_base_ {};

template <typename T>
class registered_fields_ : public registered_fields_base_ {
  friend class field_registry_impl_;

  field_map<T> fields_;
};

struct field_registry_impl_ {
  using value_type = std::unique_ptr<registered_fields_base_>;

  std::map<std::type_index, value_type> registry_;

  template <typename T>
  registered_fields_<T> *get_registered_fields_(void) const {
    auto search = this->registry_.find(typeid(T));
    if (search == std::end(this->registry_)) {
      return nullptr;
    } else {
      return static_cast<registered_fields_<T> *>((search->second).get());
    }
  }

public:
  template <typename T>
  void emplace(const char *name, const converter_fn_t<T> &fn) {
    auto *regd = this->get_registered_fields_<T>();
    if (regd == nullptr) {
      regd = new registered_fields_<T>;
      this->registry_.emplace(typeid(T), regd);
    }
    regd->fields_.emplace(name, fn);
  }

  template <typename T> const field_map<T> *get_fields(void) {
    auto *regd = this->get_registered_fields_<T>();
    if (regd == nullptr) {
      return nullptr;
    } else {
      return &(regd->fields_);
    }
  }
};

EIR_GENERATE_SINGLETON(field_registry_impl_, field_registry_);

template <typename T> inline auto fields_for(void) {
  if constexpr (std::is_class_v<T>) {
    return EIR_ACCESS_SINGLETON(field_registry_).get_fields<T>();
  } else {
    return nullptr;
  }
}

struct field_registrar_ {
  template <typename T>
  field_registrar_(const char *name, const converter_fn_t<T> &fn) {
    EIR_ACCESS_SINGLETON(field_registry_).emplace(name, fn);
  }
};

template <std::size_t N, typename T>
inline void operator|(converter &c, std::tuple<char const (&)[N], T &> val) {
  auto fields = fields_for<T>();
  if (fields == nullptr) {
    using convert_type = convert<T>;
    if constexpr (is_complete_v<convert_type>) {
      convert_type()(c, std::get<0>(val), std::get<1>(val));
    }
  } else {
    if constexpr (std::is_class_v<T>) {
      c.enter(std::get<0>(val));
      auto *t = &(std::get<1>(val));
      for (auto &[_, fn] : *fields) {
        (t->*fn)(c);
      }
      c.leave(std::get<0>(val));
    }
  }
}
} // namespace eir

#endif
