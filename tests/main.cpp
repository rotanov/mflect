#include <iostream>

#include "mflect/mflect.hpp"
#include "test_classes.hpp"

std::string mflect::type_info::GetTypeDescriptionString(const std::string& typeName, unsigned depth)
{
  using namespace mflect;
  if (db().find(typeName) == db().end())
      return "Type info record for type \"" + typeName + "\" doesn't exist.\n";

  type_info& typeInfo = *db().at(typeName);
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

      if (p.is_array())
          r += "array, ";
      if (p.is_pointer())
          r += "pointer, ";
      r += eolStr;
  }

  if (typeInfo.base() != nullptr
      && typeInfo.base() != &typeInfo)
  {
      r += eolStr;
      std::string baseName = typeInfo.base()->name();
      r += identStr1 + "Derived from type: \"" + baseName + "\"" + eolStr;
      r += type_info::GetTypeDescriptionString(baseName, depth + 1);
  }

  return r;
}

using namespace std;

int main()
{
  mflect::initialize();


#define TEST(EXPRESSION) \
  if (EXPRESSION) \
  { \
    std::cout << "test: pass, line: " << __LINE__ << std::endl; \
  } \
  else \
  { \
    std::cout << "test: fail, line: " << __LINE__ << std::endl; \
  } \

#define TYPE_INFO(TYPE) \
  mflect::type_info::find_type_info(#TYPE) \

  Foo foo;
  Bar bar;
  Foo* ptrBar = &bar;
  TEST(foo.type_info_run_time()->name() == std::string("Foo"));
  TEST(bar.type_info_run_time()->name() == std::string("Bar"));
  TEST(ptrBar->type_info_run_time()->name() == std::string("Bar"));
  TEST(ptrBar->type_info_run_time()->is_kind_of(TYPE_INFO(Bar)));
  TEST(ptrBar->type_info_run_time()->is_kind_of(TYPE_INFO(Foo)));
  TEST(MFLECT_CAST(&foo, Bar) == nullptr);
  TEST(MFLECT_CAST(ptrBar, Foo) != nullptr);
  TEST(MFLECT_CAST(ptrBar, Bar) != nullptr);
  TEST(TYPE_INFO(Foo) != nullptr);
  TEST(TYPE_INFO(Bar) != nullptr);
  TEST(TYPE_INFO(Foo)->base() == nullptr);
  TEST(TYPE_INFO(Bar)->base() == TYPE_INFO(Foo));
  TEST(TYPE_INFO(A_Foo)->base() == TYPE_INFO(Foo));
  TEST(TYPE_INFO(B_A)->base() == TYPE_INFO(A_Foo));
  TEST(TYPE_INFO(C_B)->base() == TYPE_INFO(B_A));
  {
    Vector2 foo;
    foo.x = 2.0f;
    TEST(TYPE_INFO(Vector2)->find_property("X") != nullptr);
    TEST(TYPE_INFO(Vector2)->find_property("Y") != nullptr);
    float value = 0.0f;
    void* ptrValue = &value;
    TYPE_INFO(Vector2)->find_property("X")->GetValue(&foo, ptrValue);
    TEST(value == 2.0f);
    value = 4.0f;
    TYPE_INFO(Vector2)->find_property("X")->SetValue(&foo, &value);
    TEST(foo.x == 4.0f);
    auto def_attr = mflect::property_attribute_default::get(TYPE_INFO(Vector2)->find_property("Y"));
    TEST(def_attr != nullptr)
    TEST(def_attr->test(&foo) == true);
  }

  return 0;
}

