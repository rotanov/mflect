#include <iostream>

#include "test_classes.hpp"

std::string mflect::type_info::GetTypeDescriptionString(const std::string& typeName, unsigned depth)
{
  using namespace mflect;
  if (type_info_register()[typeName] == 0)
      return "Type info record for type \"" + typeName + "\" doesn't exist.\n";

  type_info& typeInfo= *type_info_register()[typeName];
  std::string identStr(depth, '\t');
  std::string identStr1 = identStr + "\t";
  std::string identStr2 = identStr + "\t\t";
  std::string identStr3 = identStr + "\t\t\t";
  std::string eolStr = "\n";
  std::string r;

  r = identStr + "Description of type\"" + typeName + "\":" + eolStr + eolStr +
      identStr1 + "name: " + typeInfo.name() + eolStr + eolStr +
      identStr1 + "Properties:" + eolStr;

  for (auto i : typeInfo.properties())
  {
      property_info& p = *(i.second);
      r += identStr2 + "key: \"" + (i.first) + "\"" + eolStr +
           identStr3 + "name: \"" + p.name() + "\"" + eolStr +
           identStr3 + "type name: \"" + p.type_name() + "\"" + eolStr +
           identStr3 + "owner name: \"" + p.owner_type_name() + "\"" + eolStr +
           identStr3 + "valid flags: ";

      if (p.IsArray())
          r += "array, ";
      if (p.IsPointer())
          r += "pointer, ";
      if (p.Integral())
          r += "integral, ";
      r += eolStr;
  }

  if (typeInfo.base_info() != nullptr
      && typeInfo.base_info() != &typeInfo)
  {
      r += eolStr;
      std::string baseName = typeInfo.base_info()->name();
      r += identStr1 + "Derived from type: \"" + baseName + "\"" + eolStr;
      r += type_info::GetTypeDescriptionString(baseName, depth + 1);
  }

  return r;
}

using namespace std;

MFLECT_TYPE_INFO_DECLARE(Foo)

int main()
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
  std::cout << MFLECT_CAST(&bar, Foo) << std::endl;
  std::cout << MFLECT_CAST(&bar, Bar) << std::endl;

//  class Foo
//  {
//  public:
//    virtual void derp() = 0;
//  };

//  class Bar : public Foo
//  {

//  };

//  class Hurr : public Foo
//  {
//  public:
//    virtual void derp()
//    {

//    }
//  };

//  try
//  {
//    auto i = mflect::make<Foo>::instance();
//  }
//  catch (runtime_error& e)
//  {
//    cout << e.what() << endl;;
//  }

//  try
//  {
//    auto i = mflect::make<Bar>::instance();
//  }
//  catch (runtime_error& e)
//  {
//    cout << e.what() << endl;;
//  }

//  try
//  {
//    auto i = mflect::make<Hurr>::instance();
//  }
//  catch (runtime_error& e)
//  {
//    cout << e.what() << endl;
//  }

//  try
//  {
//    auto i = mflect::make<Foo*>::instance();
//  }
//  catch (runtime_error& e)
//  {
//    cout << e.what() << endl;
//  }

//  cout << mflect::is_integral<string>::value;
//  cout << mflect::is_integral<Foo>::value;

//  cout << endl << mflect::type_info::GetTypeDescriptionString("Foo") << endl;
  return 0;
}

