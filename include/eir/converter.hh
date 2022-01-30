#ifndef EIR_CONVERTER_HH
#define EIR_CONVERTER_HH

#define EIR_CONVERT_PROPERTY(name) convert_##name##_

#include <memory>
#include <string>
#include <tuple>

namespace eir {

enum class direction { INPUT, OUTPUT };

struct converter {
  direction dir;
  converter(direction dir_) : dir(dir_) {}

  virtual void enter(const char *) {}
  virtual void leave(const char *) {}
  virtual std::string read(void) { return ""; }
  virtual void write(const std::string &) {}

  bool is_writing(void) const { return (this->dir == direction::OUTPUT); }
};

template <typename T> using converter_fn_t = void (T::*)(converter &);

template <typename T, typename Enable = void> struct convert;

template <> struct convert<int> {
  void operator()(converter &c, const char *field, int &val) const {
    c.enter(field);
    if (c.is_writing()) {
      c.write(std::to_string(val));
    } else {
      val = std::stoi(c.read());
    }
    c.leave(field);
  }
};

template <typename T> struct convert<std::unique_ptr<T>> {
  void operator()(converter &c, const char *field,
                  std::unique_ptr<T> &val) const {
    if (!c.is_writing()) {
      auto blk = ::operator new(sizeof(T));
      val.reset((T *)blk);
    }
    convert<T>()(c, field, *val);
  }
};
} // namespace eir

#endif
