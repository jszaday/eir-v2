#ifndef EIR_SERIALIZABLE_HH
#define EIR_SERIALIZABLE_HH

#include <eir/singleton.hh>
#include <map>
#include <string>
#include <typeindex>

#define EIR_SERIALIZE_PROPERTY(name) serialize_##name##_

namespace eir {
template <typename T>
using serializer_fn_t = void (T::*)(void);

template <typename T>
using field_map = std::map<std::string, serializer_fn_t<T>>;

class serializer_base_ {};

template <typename T>
class serializer_ : public serializer_base_ {
  friend class field_registry_impl_;

  field_map<T> fields_;
};

struct field_registry_impl_ {
  using value_type = std::unique_ptr<serializer_base_>;

  std::map<std::type_index, value_type> registry_;

  template <typename T>
  serializer_<T>* get_serializer_(void) {
    auto search = this->registry_.find(typeid(T));
    if (search == std::end(this->registry_)) {
      search = this->registry_.emplace(typeid(T), new serializer_<T>).first;
    }
    return static_cast<serializer_<T>*>((search->second).get());
  }

 public:
  template <typename T>
  void emplace(const char* name, const serializer_fn_t<T>& fn) {
    this->get_serializer_<T>()->fields_.emplace(name, fn);
  }

  template <typename T>
  const field_map<T>& get_fields(void) {
    return this->get_serializer_<T>()->fields_;
  }
};

EIR_GENERATE_SINGLETON(field_registry_impl_, field_registry_);

template <typename T>
inline const field_map<T>& fields_of(void) {
  return EIR_ACCESS_SINGLETON(field_registry_).get_fields<T>();
}

struct field_registrar_ {
  template <typename T>
  field_registrar_(const char* name, const serializer_fn_t<T>& fn) {
    EIR_ACCESS_SINGLETON(field_registry_).emplace(name, fn);
  }
};
}  // namespace eir

#endif
