#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include <string>

#include "mflect/mflect.hpp"
#include "serialization_json.hpp"

#define DECL_TYPE(TYPE) MFLECT_TYPE_INFO_DECLARE(TYPE)
#define DECL_TYPE_DERIVED(TYPE_DERIVED, TYPE_BASE) \
  MFLECT_TYPE_INFO_DECLARE_DERIVED(TYPE_DERIVED, TYPE_BASE) \

#define DECL_PROP(OWNER, TYPE, NAME) \
  MFLECT_DECLARE_PROPERTY_INFO(OWNER, TYPE, NAME)

#define DECL_DEF(OWNER, NAME, VALUE) \
  MFLECT_DECLARE_ATTRIBUTE_DEFAULT(OWNER, NAME, VALUE)

class Vector2
{
  MFLECT_INJECT_TYPE_INFO_DERIVED(Vector2, Vector2);

public:
  float x;
  float y;

  Vector2()
    : x(0.0f)
    , y(0.0f)
  {

  }

  virtual ~Vector2()
  {

  }

  float GetX() const { return x; }
  float GetY() const { return y; }
  void SetX(const float value) { x = value; }
  void SetY(const float value) { y = value; }
};

class Vector3 : public Vector2
{
  friend class anoos;
  MFLECT_INJECT_TYPE_INFO_DERIVED(Vector3, Vector2);

public:
  float z;

  float GetZ() const { return z; }
  void SetZ(const float value) { z = value; }
};

DECL_TYPE(float)

DECL_TYPE_DERIVED(Vector2, Vector2)
  DECL_PROP(Vector2, float, X)
    DECL_DEF(Vector2, X, 0.0f)
  DECL_PROP(Vector2, float, Y)
    DECL_DEF(Vector2, Y, 0.0f)

DECL_TYPE_DERIVED(Vector3, Vector2)
  DECL_PROP(Vector3, float, Z)
    DECL_DEF(Vector3, Z, 0.0f)

int main(int /*argc*/, char* /*argv*/[])
{
  mflect::type_info::initialize();

  Vector3 v3;
  v3.x = 1.0f;
  v3.y = 2.0f;
  v3.z = 3.0f;
  Vector2* v2 = &v3;
  char* buffer = nullptr;

  try
  {
    persistence::to_json(v2, "Vector2", buffer);
    std::string typeName = persistence::extract_type_name(buffer);
    std::cout << buffer << std::endl;
    std::cout << typeName << std::endl;
    void* next = mflect::type_info::find_type_info(typeName)->make_new();
    persistence::from_json(next, typeName, buffer);
    Vector3* ptr = static_cast<Vector3*>(next);
    std::cout << ptr->x << " " << ptr->y << " " << ptr->z << std::endl;
  }
  catch (std::runtime_error& e)
  {
    std::cout << e.what() << std::endl;
  }
  return EXIT_SUCCESS;
}
