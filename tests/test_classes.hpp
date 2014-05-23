#pragma once

#include "mflect/mflect.hpp"

class Foo
{
  MFLECT_INJECT_TYPE_INFO(Foo)
};

class Bar : public Foo
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(Bar, Foo)
};
