#include <iostream>
#include <eir/property.hh>
#include <eir/xml_reader.hh>
#include <eir/xml_writer.hh>
#include <sstream>
#include <memory>
#include <vector>

class animal {
 public:
  virtual ~animal() {}

  virtual int num_legs(void) const = 0;
  virtual const char* noise(void) const = 0;

  template <typename OS>
  void make_noise(OS& os) const {
    os << this->noise() << "!" << std::endl;
  }
};

class biped : public animal {
  virtual int num_legs(void) const override { return 4; }
};

class quadraped : public animal {
  virtual int num_legs(void) const override { return 4; }
};

class bird : public biped {};

class canadian_goose : public bird {
  virtual const char* noise(void) const override { return "honk"; }
};

EIR_CLASS_STUB(canadian_goose);

class lion : public quadraped {
  virtual const char* noise(void) const override { return "roar"; }
};

EIR_CLASS_STUB(lion);

class dog : public quadraped {
 private:
  std::string breed_;

 public:
  template <typename... Args>
  dog(Args&&... args) : breed_(std::forward<Args>(args)...) {}

  EIR_PROPERTY_ACCESSOR(dog, breed);

  virtual const char* noise(void) const override { return "bark"; }
};

int main(void) {
  std::vector<std::unique_ptr<animal>> animals;

  animals.emplace_back(new dog("corgi"));
  animals.emplace_back(new canadian_goose);
  animals.emplace_back(new lion);

  std::cout << "the animals say:" << std::endl;
  for (auto& animal : animals) {
    animal->make_noise(std::cout);
  }

  std::stringstream ss;

  eir::xml_writer w(ss);
  w | std::tie("animals", animals);

  auto xml = ss.str();
  std::cout << xml;

  std::vector<std::unique_ptr<animal>> clones;
  eir::xml_reader r(xml.c_str());
  r | std::tie("animals", clones);

  std::cout << "the clones say:" << std::endl;
  for (auto& animal : clones) {
    animal->make_noise(std::cout);
  }

  return 0;
}
