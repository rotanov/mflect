#include <cstdlib>
#include <iostream>
#include <functional>
#include <ctime>
#include <iostream>

#include "mflect/mflect.hpp"

#include "test_classes.hpp"

void SetHighPriority();

#if defined(WIN32) || defined(_WIN32)
#include "windows.h"

void SetHighPriority()
{
  if(!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
  {
    std::cout << "fail to set process priority" << std::endl;
  }
}
#endif // WIN32

int main(int /*argc*/, char* /*argv*/[])
{
  SetHighPriority();
  mflect::initialize();

  auto measure = [](std::function<void()> f)
  {
    const int totalAttempts = 10;
    double total = 0.0;
    for (int tries = 0; tries < totalAttempts; tries++)
    {
      std::cout << tries << "..";
      clock_t begin = clock();
      f();
      clock_t end = clock();
      total += double(end - begin) / CLOCKS_PER_SEC;
    }
    std::cout << "elapsed " << total / totalAttempts << std::endl;
  };

  F_E f_e_inst;
  Foo foo_inst;
  const int castTries = 10000000;
  F_E* resultPtr = nullptr;
  measure([&]()
  {
    for (int i = 0; i < castTries; i++)
    {
      Foo* ptrFoo = &f_e_inst;
      F_E* ptrF_E = dynamic_cast<F_E*>(ptrFoo);
      resultPtr = ptrF_E;
    }
  });

  std::cout << resultPtr << std::endl;

  measure([&]()
  {
    for (int i = 0; i < castTries; i++)
    {
      Foo* ptrFoo = &f_e_inst;
      F_E* ptrF_E = MFLECT_CAST(ptrFoo, F_E);
      resultPtr = ptrF_E;
    }
  });

  std::cout << resultPtr << std::endl;

  mflect::type_info* typeInfoFoo = mflect::type_info::find_type_info("Foo");
  mflect::type_info* typeInfoF_E = mflect::type_info::find_type_info("F_E");

  measure([&]()
  {
    for (int i = 0; i < castTries; i++)
    {
      Foo* ptrFoo = &f_e_inst;
      F_E* ptrF_E = ptrFoo->type_info_run_time()->cast<F_E>(ptrFoo, typeInfoF_E);
      resultPtr = ptrF_E;
    }
  });

  std::cout << resultPtr << std::endl;

  return EXIT_SUCCESS;
}
