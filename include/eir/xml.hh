#ifndef EIR_XML_HH
#define EIR_XML_HH

#include <map>
#include <string>
#include <variant>

namespace eir {
template <class>
inline constexpr bool always_false_v = false;

template <typename OS>
class xml_writer;

struct xml_data {
  std::map<std::string, std::variant<std::string, xml_data>> entities;

  template <typename... Args>
  xml_data(Args&&... args) : entities(std::forward<Args>(args)...) {}

  template <typename OS>
  void write_to(xml_writer<OS>&) const;
};

template <typename OS>
class xml_writer {
  OS& os_;
  int num_tabs_;

  void write_tabs_(void) {
    for (auto i = 0; i < num_tabs_; i++) {
      this->os_ << '\t';
    }
  }

 public:
  xml_writer(OS& os) : os_(os), num_tabs_(0) {}

  xml_writer& start_tag(const std::string& s) {
    this->write_tabs_();

    this->os_ << '<' << s << ">\n";

    num_tabs_ += 1;

    return *this;
  }

  xml_writer& writeln(const std::string& s) {
    this->write_tabs_();

    this->os_ << s << '\n';

    return *this;
  }

  xml_writer& end_tag(const std::string& s) {
    num_tabs_ -= 1;

    this->write_tabs_();

    this->os_ << "</" << s << ">\n";

    return *this;
  }
};

template <typename OS>
void xml_data::write_to(xml_writer<OS>& w) const {
  for (auto& pair : this->entities) {
    w.start_tag(pair.first);

    std::visit(
        [&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, std::string>) {
            w.writeln(arg);
          } else if constexpr (std::is_same_v<T, xml_data>) {
            arg.write_to(w);
          } else {
            static_assert(always_false_v<T>, "non-exhaustive visitor!");
          }
        },
        pair.second);

    w.end_tag(pair.first);
  }
}

}  // namespace eir

#endif
