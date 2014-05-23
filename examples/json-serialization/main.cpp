#include <cstdlib>
#include <iostream>
#include <typeinfo>

#include "mflect/mflect.hpp"

class Foo
{
public:
  virtual ~Foo()
  {

  }
};

class Bar : public Foo
{
};

int main(int /*argc*/, char* /*argv*/[])
{
  Foo* foo = new Bar;
//  Bar* bar_ptr = dynamic_cast<Bar*>(foo);
//  std::cout << bar_ptr << std::endl;
//  std::cout << typeid(foo).name() << std::endl;
  return EXIT_SUCCESS;
}
