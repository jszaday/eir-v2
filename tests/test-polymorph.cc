#include <cassert>
#include <eir/property.hh>
#include <eir/xml.hh>
#include <iostream>
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

class lion : public quadraped {
  virtual const char* noise(void) const override { return "roar"; }
};

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

  for (auto& animal : animals) {
    animal->make_noise(std::cout);
  }

  eir::xml_writer w(std::cout);
  w | std::tie("animals", animals);

  return 0;
}
