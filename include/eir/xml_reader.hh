#ifndef EIR_XML_READER_HH
#define EIR_XML_READER_HH

#include <tinyxml2/tinyxml2.h>

#include <cassert>
#include <cstring>
#include <eir/converter.hh>
#include <istream>
#include <iterator>
#include <map>
#include <stack>
#include <string>
#include <variant>

namespace eir {
class xml_reader : public converter {
  tinyxml2::XMLDocument doc_;
  std::stack<std::pair<const char *, tinyxml2::XMLElement *>> stack_;

 public:
  xml_reader(const char *xml) : converter(direction::INPUT) {
    this->doc_.Parse(xml);
  }

  template <typename IS>
  xml_reader(IS &is) : converter(direction::INPUT) {
    auto skipws = is.flags() & std::ios_base::skipws;

    if (skipws) {
      is >> std::noskipws;
    }

    std::istream_iterator<char> it(is);
    std::istream_iterator<char> end;
    std::string text(it, end);

    this->doc_.Parse(text.c_str());

    if (skipws) {
      is >> std::skipws;
    }
  }

  virtual std::string read(void) override {
    auto &top = this->stack_.top();
    return (top.second)->Value();
  }

  virtual void enter(
      const char *field,
      std::map<std::string, std::string> *attributes = nullptr) override {
    tinyxml2::XMLElement *elt;
    if (this->stack_.empty()) {
      elt = (this->doc_).FirstChildElement(field);
    } else {
      auto &top = this->stack_.top();
      elt = (top.second)->FirstChildElement(field);
    }

    this->stack_.emplace(field, elt);

    if (attributes) {
      auto *attribute = elt->FirstAttribute();
      while (attribute) {
        attributes->emplace(attribute->Name(), attribute->Value());
        attribute = attribute->Next();
      }
    }
  }

  virtual void leave(const char *field) override {
    auto &top = this->stack_.top();
    assert(strcmp(field, top.first) == 0);
    this->stack_.pop();
  }
};
}  // namespace eir

#endif
