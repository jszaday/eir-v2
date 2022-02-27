#ifndef EIR_COMMON_HH
#define EIR_COMMON_HH

#include <map>
#include <memory>
#include <string>

namespace eir {
struct converter;

using properties_t = std::map<std::string, std::string>;
using allocator_fn_t = void *(*)(void);
using generic_converter_t = void (*)(converter *, void *);

class registered_fields_base_ {};

struct class_stub_ {
  const char *name;
  allocator_fn_t allocator;
  generic_converter_t converter;
  std::unique_ptr<registered_fields_base_> fields;

  template <typename... Args>
  class_stub_(const char *name_, allocator_fn_t allocator_,
              generic_converter_t converter_, Args &&...args)
      : name(name_), allocator(allocator_), converter(converter_),
        fields(std::forward<Args>(args)...) {}
};

const class_stub_ *get_stub_by_name(const std::string &name);
template <typename T> const class_stub_ *get_stub_by_type(const T &t);
} // namespace eir

#endif
