#ifndef EIR_SINGLETON_HH
#define EIR_SINGLETON_HH

#include <memory>

namespace eir {
template <typename T, typename SingletonClass>
class singleton {
 public:
  using value_type = T;
  using class_type = SingletonClass;

  // Non-copyable, non-movable
  singleton(singleton const&) = delete;
  singleton(singleton&&) = delete;
  singleton& operator=(singleton const&) = delete;
  singleton& operator=(singleton&&) = delete;

  static const std::unique_ptr<value_type>& instance() {
    static std::unique_ptr<value_type> inst{new value_type()};

    return inst;
  }

 protected:
  singleton() = default;
};
}  // namespace eir

#define EIR_GENERATE_SINGLETON(type, name)         \
  class name : public eir::singleton<type, name> { \
   private:                                        \
    name() = default;                              \
  }

#define EIR_ACCESS_SINGLETON(name) (*name::instance())

#endif
