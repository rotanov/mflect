#include "test_classes.hpp"

#define DECL_TYPE(TYPE) MFLECT_TYPE_INFO_DECLARE(TYPE)
#define DECL_TYPE_DERIVED(TYPE_DERIVED, TYPE_BASE) \
  MFLECT_TYPE_INFO_DECLARE_DERIVED(TYPE_DERIVED, TYPE_BASE) \

#define DECL_PROP(OWNER, TYPE, NAME) \
  MFLECT_DECLARE_PROPERTY_INFO(OWNER, TYPE, NAME)

#define DECL_DEF(OWNER, NAME, VALUE) \
  MFLECT_DECLARE_ATTRIBUTE_DEFAULT(OWNER, NAME, VALUE)

DECL_TYPE(Foo)
DECL_TYPE_DERIVED(Bar, Foo)
DECL_TYPE_DERIVED(A_Foo, Foo)
DECL_TYPE_DERIVED(B_A, A_Foo)
DECL_TYPE_DERIVED(C_B, B_A)
DECL_TYPE_DERIVED(D_C, C_B)
DECL_TYPE_DERIVED(E_D, D_C)
DECL_TYPE_DERIVED(F_E, E_D)

DECL_TYPE(Vector2)
  DECL_PROP(Vector2, float, X)
    DECL_DEF(Vector2, X, 0.0f)
  DECL_PROP(Vector2, float, Y)
    DECL_DEF(Vector2, Y, 0.0f)
