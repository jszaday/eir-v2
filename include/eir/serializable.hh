#ifndef EIR_SERIALIZABLE_HH
#define EIR_SERIALIZABLE_HH

#include <cassert>
#include <eir/converter.hh>
#include <eir/singleton.hh>
#include <eir/traits.hh>
#include <map>
#include <string>
#include <typeindex>
#include <utility>

namespace eir {
template <typename T> using field_map = std::map<std::string, convert_fn_t<T>>;

template <typename T>
class registered_fields_ : public registered_fields_base_ {
  friend class field_registry_impl_;

  field_map<T> fields_;
};

struct field_registry_impl_ {
  std::map<std::string, const class_stub_ *> names_;
  std::map<std::type_index, class_stub_> stubs_;

  template <typename T>
  registered_fields_<T> *get_registered_fields_(void) const {
    auto *stub = this->stub_for(typeid(T));
    return stub ? static_cast<registered_fields_<T> *>(stub->fields.get())
                : nullptr;
  }

public:
  template <typename T>
  registered_fields_<T> *emplace(const char *class_name,
                                 allocator_fn_t allocator,
                                 generic_converter_t converter) {
    auto *fields = this->get_registered_fields_<T>();
    if (fields == nullptr) {
      fields = new registered_fields_<T>;
      auto ins = this->stubs_.emplace(
          std::piecewise_construct, std::forward_as_tuple(typeid(T)),
          std::forward_as_tuple(class_name, allocator, converter, fields));
      this->names_.emplace(class_name, &(ins.first->second));
    }
    return fields;
  }

  template <typename T>
  void emplace(const char *field_name, const convert_fn_t<T> &fn) {
    auto *fields = this->get_registered_fields_<T>();
    assert((bool)fields);
    fields->fields_.emplace(field_name, fn);
  }

  template <typename T> const field_map<T> *get_fields(void) {
    auto *regd = this->get_registered_fields_<T>();
    if (regd == nullptr) {
      return nullptr;
    } else {
      return &(regd->fields_);
    }
  }

  const class_stub_ *stub_for(const std::type_index &index) const {
    auto search = this->stubs_.find(index);
    if (search == std::end(this->stubs_)) {
      return nullptr;
    } else {
      return &(search->second);
    }
  }

  const class_stub_ *stub_for(const std::string &name) const {
    auto search = this->names_.find(name);
    if (search == std::end(this->names_)) {
      return nullptr;
    } else {
      return search->second;
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

template <typename T> struct stub_registrar_ {
  using self_type = stub_registrar_<T>;

  stub_registrar_(const char *class_name) {
    EIR_ACCESS_SINGLETON(field_registry_)
        .emplace<T>(class_name, &self_type::allocate, &self_type::convert);
  }

  static void *allocate(void) { return ::operator new(sizeof(T)); }

  static void convert(converter *c, void *raw) {
    auto *t = (T *)raw;
    if (c->is_reading()) {
      new (t) T;
    }
    auto *fields = fields_for<T>();
    for (auto &[_, fn] : *fields) {
      (t->*fn)(*c);
    }
  }
};

template <typename T> struct field_registrar_ {
  stub_registrar_<T> stub;

  field_registrar_(const char *class_name, const char *field_name,
                   const convert_fn_t<T> &fn)
      : stub(class_name) {
    EIR_ACCESS_SINGLETON(field_registry_).emplace(field_name, fn);
  }
};

template <typename T> const class_stub_ *get_stub_by_type(const T &t) {
  auto &singleton = EIR_ACCESS_SINGLETON(field_registry_);
  if constexpr (std::is_polymorphic_v<T>) {
    return singleton.stub_for(typeid(t));
  } else {
    return singleton.stub_for(typeid(T));
  }
}

const class_stub_ *get_stub_by_name(const std::string &name) {
  return EIR_ACCESS_SINGLETON(field_registry_).stub_for(name);
}

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
