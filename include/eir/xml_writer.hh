#ifndef EIR_XML_WRITER_HH
#define EIR_XML_WRITER_HH

#include <tinyxml2/tinyxml2.h>

#include <cassert>
#include <cstring>
#include <eir/converter.hh>
#include <istream>
#include <iterator>
#include <stack>

namespace eir {
class xml_writer : public converter {
  tinyxml2::XMLDocument doc_;
  std::stack<std::pair<const char *, tinyxml2::XMLElement *>> stack_;

 public:
  xml_writer(void) : converter(direction::OUTPUT) {}

  virtual void enter(
      const char *field,
      std::map<std::string, std::string> *attributes = nullptr) override {
    auto *elt = this->doc_.NewElement(field);

    if (this->stack_.empty()) {
      (this->doc_).InsertEndChild(elt);
    } else {
      auto &top = this->stack_.top();
      (top.second)->InsertEndChild(elt);
    }

    if (attributes) {
      for (auto &[key, val] : *attributes) {
        elt->SetAttribute(key.c_str(), val.c_str());
      }
    }

    this->stack_.emplace(field, elt);
  }

  virtual void leave(const char *field) override {
    auto &top = this->stack_.top();
    assert(strcmp(field, top.first) == 0);
    this->stack_.pop();
  }

  virtual void write(const std::string &val) override {
    auto &top = this->stack_.top();
    (top.second)->SetText(val.c_str());
  }

  std::string str(void) const {
    tinyxml2::XMLPrinter printer;
    (this->doc_).Print(&printer);
    return printer.CStr();
  }
};
}  // namespace eir

#endif
