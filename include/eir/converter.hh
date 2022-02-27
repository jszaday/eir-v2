#ifndef EIR_CONVERTER_HH
#define EIR_CONVERTER_HH

#define EIR_CONVERT_PROPERTY(name) convert_##name##_

#include <eir/common.hh>
#include <map>
#include <sstream>
#include <tuple>
#include <vector>

namespace eir {

enum class direction { INPUT, OUTPUT };

struct converter {
  direction dir;
  converter(direction dir_) : dir(dir_) {}

  virtual void enter(const char *field, properties_t *properties = nullptr) {}
  virtual void leave(const char *field) {}
  virtual std::string read(void) { return ""; }
  virtual void write(const std::string &) {}

  bool is_reading(void) const { return (this->dir == direction::INPUT); }
  bool is_writing(void) const { return (this->dir == direction::OUTPUT); }
};

template <typename T> using convert_fn_t = void (T::*)(converter &);

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

template <> struct convert<std::string> {
  void operator()(converter &c, const char *field, std::string &val) const {
    c.enter(field);
    if (c.is_reading()) {
      val = c.read();
    } else {
      c.write(val);
    }
    c.leave(field);
  }
};

template <typename T>
struct convert<std::unique_ptr<T>,
               std::enable_if_t<!std::is_polymorphic_v<T>>> {
  void operator()(converter &c, const char *field,
                  std::unique_ptr<T> &val) const {
    if (!c.is_writing()) {
      auto blk = ::operator new(sizeof(T));
      val.reset((T *)blk);
    }
    convert<T>()(c, field, *val);
  }
};

template <typename T>
struct convert<std::unique_ptr<T>, std::enable_if_t<std::is_polymorphic_v<T>>> {
  void operator()(converter &c, const char *field,
                  std::unique_ptr<T> &val) const {
    std::map<std::string, std::string> properties;
    const class_stub_ *stub;
    if (c.is_writing()) {
      stub = get_stub_by_type(*val);
      properties.emplace("kind", stub->name);
    }
    c.enter(field, &properties);
    if (c.is_reading()) {
      stub = get_stub_by_name(properties["kind"]);
      val.reset((T *)stub->allocator());
    }
    stub->converter(&c, val.get());
    c.leave(field);
  }
};

template <typename T> struct convert<std::vector<T>> {
  void operator()(converter &c, const char *field, std::vector<T> &val) const {
    std::map<std::string, std::string> properties;
    if (c.is_writing()) {
      properties.emplace("size", std::to_string(val.size()));
    }
    c.enter(field, &properties);
    auto size = std::stoull(properties["size"]);
    if (c.is_reading()) {
      // TODO ( assumes default-constructible! )
      val.resize(size);
    }
    for (auto i = 0; i < size; i++) {
      std::stringstream ss;
      ss << "element-" << i;
      convert<T>()(c, ss.str().c_str(), val[i]);
    }
    c.leave(field);
  }
};
} // namespace eir

#endif
