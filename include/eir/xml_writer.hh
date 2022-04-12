#ifndef EIR_XML_WRITER_HH
#define EIR_XML_WRITER_HH

#include <eir/converter.hh>
#include <map>
#include <string>

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

  // TODO ( implement this! )
  static std::string escape(const std::string &value) { return value; }

  xml_writer &start_tag(const std::string &s,
                        std::map<std::string, std::string> *attributes) {
    this->write_tabs_();

    this->os_ << '<' << s;

    if (attributes) {
      for (auto &pair : *attributes) {
        this->os_ << ' ' << pair.first << "=\"" << escape(pair.second) << '"';
      }
    }

    this->os_ << ">\n";

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

  virtual void
  enter(const char *field,
        std::map<std::string, std::string> *attributes = nullptr) override {
    this->start_tag(field, attributes);
  }

  virtual void leave(const char *field) override { this->end_tag(field); }

  virtual void write(const std::string &val) override { this->writeln(val); }
};
} // namespace eir

#endif
