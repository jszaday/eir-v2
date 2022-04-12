#include <cassert>
#include <eir/property.hh>
#include <eir/xml_writer.hh>
#include <iostream>
#include <memory>

class example {
  int trivial_;

  std::unique_ptr<int> non_trivial_;

 public:
  int num_assigns = 0;

  EIR_PROPERTY_ACCESSOR(example, trivial);

  EIR_PROPERTY_ACCESSOR(example, non_trivial);

  void on_assign(void) {
    this->num_assigns += 1;

    std::cout << "a property was assigned!" << std::endl;
  }
};

int main(void) {
  example ex;

  auto trivial = (ex.trivial() = 21);

  ex.non_trivial() = std::unique_ptr<int>(new int(trivial));
  trivial.apply([](int& lval) { lval *= 2; });

  assert(ex.num_assigns == 3);

  eir::xml_writer w(std::cout);
  w | std::tie("example", ex);

  return 0;
}
