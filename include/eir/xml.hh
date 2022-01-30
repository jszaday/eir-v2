#ifndef EIR_XML_HH
#define EIR_XML_HH

#include <eir/converter.hh>
#include <map>
#include <string>
#include <variant>

namespace eir {
template <typename OS> class xml_writer : public converter {
  OS &os_;
  int num_tabs_;

  void write_tabs_(void) {
    for (auto i = 0; i < num_tabs_; i++) {
      this->os_ << '\t';
    }
  }

public:
  xml_writer(OS &os) : converter(direction::OUTPUT), os_(os), num_tabs_(0) {}

  xml_writer &start_tag(const std::string &s) {
    this->write_tabs_();

    this->os_ << '<' << s << ">\n";

    num_tabs_ += 1;

    return *this;
  }

  xml_writer &writeln(const std::string &s) {
    this->write_tabs_();

    this->os_ << s << '\n';

    return *this;
  }

  xml_writer &end_tag(const std::string &s) {
    num_tabs_ -= 1;

    this->write_tabs_();

    this->os_ << "</" << s << ">\n";

    return *this;
  }

  virtual void enter(const char *field) override { this->start_tag(field); }

  virtual void leave(const char *field) override { this->end_tag(field); }

  virtual void write(const std::string &val) override { this->writeln(val); }
};
} // namespace eir

#endif
