#pragma once

#include "mflect/mflect.hpp"

class Foo
{
  MFLECT_INJECT_TYPE_INFO(Foo)

public:
  virtual ~Foo()
  {

  }
};

class Bar : public Foo
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(Bar, Foo)
};

class A_Foo : public Foo
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(A_Foo, Foo)
};

class B_A : public A_Foo
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(B_A, A_Foo)
};

class C_B : public B_A
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(C_B, B_A)
};

class D_C : public C_B
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(D_C, C_B)
};

class E_D : public D_C
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(E_D, D_C)
};

class F_E : public E_D
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(F_E, E_D)
};

class Vector2
{
public:
  float x;
  float y;

  Vector2()
    : x(0.0f)
    , y(0.0f)
  {

  }

  float GetX() const { return x; }
  float GetY() const { return y; }
  void SetX(const float value) { x = value; }
  void SetY(const float value) { y = value; }
};
