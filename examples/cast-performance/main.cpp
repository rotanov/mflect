#include <cstdlib>
#include <iostream>

#include "mflect/mflect.hpp"

class Foo
{
  MFLECT_INJECT_TYPE_INFO(Foo)
};

class Bar : public Foo
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(Bar, Foo)
};

MFLECT_TYPE_INFO_DECLARE(Foo)
MFLECT_TYPE_INFO_DECLARE_DERIVED(Bar, Foo)

int main(int /*argc*/, char* /*argv*/[])
{
  mflect::type_info::initialize();

  Foo foo;
  Bar bar;
  std::cout << foo.GetTypeInfo()->name() << std::endl;
  std::cout << bar.GetTypeInfo()->name() << std::endl;
  Foo* bar_ptr = &bar;
  std::cout << bar_ptr->GetTypeInfo()->name() << std::endl;
  std::cout << bar_ptr->GetTypeInfo()->is_kind_of("Bar") << std::endl;
  std::cout << bar_ptr->GetTypeInfo()->is_kind_of("Foo") << std::endl;
  std::cout << MFLECT_CAST(&foo, Bar) << std::endl;
  std::cout << MFLECT_CAST(&bar, Bar) << std::endl;
  return EXIT_SUCCESS;
}
