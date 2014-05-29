/*******************************************************************************
 * Copyright (C) 2014 Denis Rotanov <drotanov@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/

#pragma once

#include <unordered_map>
#include <string>

namespace mflect
{

class property_info;
/**
 * @brief The type_info class
 */
class type_info
{
public:
  type_info()
    : hasDerived_(false)
  {
  }

  typedef std::unordered_map<std::string, type_info*> db_type;

  /**
   * @brief name returns string representation of this type's name.
   */
  virtual const char* name() const = 0;

  /**
   * @brief make_new makes new instance of this type.
   * @return pointer to new instance casted to void*
   *
   * If this type can't be instantiated (e.g. it's abstract) MFLECT_RUNTIME_ERROR
   * will be called. It may be predefined by user. By default it's exception
   * std::runtime_error.
   */
  virtual void* make_new() const = 0;

  /**
   * @brief property
   * @param name
   * @return
   */
  virtual property_info* property(const std::string& name) const = 0;

  /**
   * @brief properties
   * @return
   */
  virtual std::unordered_map<std::string, property_info*>& properties() const = 0;

  /**
   * @brief Get a pointer to type_info instance for base class of this one.
   * @return pointer to type_info if this type has base class, nullptr otherwise.
   */
  inline type_info* base() const;

  /**
   * @brief Get string representation of given type instance.
   * @param instance of this type
   * @return string representing the instance
   */
  virtual std::string to_string(void* instance) const = 0;

  /**
   * @brief from_string
   * @param instance
   * @param value
   */
  virtual void from_string(void* instance, const std::string& value) const = 0;

  /**
   * @brief is_integral
   * @return
   */
  virtual bool is_integral() const = 0;

  /**
   * @brief Check if this type has at least one derived type.
   * @return true if there is at least one derived type, false otherwise.
   */
  inline bool has_derived() const;

  /**
   * @brief FindProperty
   * @param name
   * @return
   */
  inline property_info *find_property(const std::string &name) const;

  /**
   * @brief GetRunTimeTypeInfo
   * @return
   */
  inline virtual type_info* type_info_run_time(const void*instance) const;

  /**
   * @brief find_type_info
   * @param typeName
   * @return
   */
  inline static type_info* find_type_info(const std::string &typeName);

  /**
   * @brief Initialize
   */
  inline static void initialize();

  /**
   * @brief is_kind_of
   * @param typeInfo
   * @return
   */
  inline bool is_kind_of(const type_info* typeInfo) const;

  /**
   * @see is_kind_of
   */
  inline bool is_kind_of(const char* typeName) const;

  /**
   * @brief GetInheritanceDepth
   */
  inline unsigned GetInheritanceDepth() const;

  /**
   * @brief GetTypeDescriptionString
   * @param typeName
   * @param depth
   * @return human-readable type summary string
   */
  static std::string GetTypeDescriptionString(const std::string& typeName, unsigned depth = 0);

  /**
   * @brief Cast
   * @param instance
   * @param typeName
   * @return
   */
  template <typename T>
  T* cast(void* instance, const char* typeName);

  /**
   * @brief type_info_register
   * @return
   */
  inline static const db_type& db();

protected:
  bool has_derived_;

};

//==============================================================================
template <typename T>
T* type_info::cast(void* instance, const char* typeName)
{
  type_info* typeInfo = GetRunTimeTypeInfo(instance);
  MFLECT_ASSERT(typeInfo != nullptr);
  if (typeInfo->is_kind_of(typeName))
  {
    return static_cast<T*>(instance);
  }
  return nullptr;
}

//==============================================================================
bool type_info::has_derived() const
{
  return hasDerived_;
}

//==============================================================================
property_info* type_info::find_property(const std::string& name) const
{
  const type_info* typeInfo = this;
  property_info* propertyInfo = nullptr;

  while (typeInfo != nullptr
         && propertyInfo == nullptr)
  {
    if (typeInfo->properties().find(name) != typeInfo->properties().end())
    {
      propertyInfo = typeInfo->property(name);
    }

    if (typeInfo == typeInfo->base())
    {
      break;
    }

    typeInfo = typeInfo->base();
  }

  return propertyInfo;
}

//==============================================================================
type_info* type_info::type_info_run_time(const void* /*instance*/) const
{
  MFLECT_RUNTIME_ERROR("type_info_run_time not implemented for type: " + std::string(name()));
}

//==============================================================================
type_info* type_info::find_type_info(const std::string& typeName)
{
  auto i = db().find(typeName);
  if (i == db().end())
  {
      MFLECT_WARNING("type_info record for type: \"" + typeName + "\" doesn't exist");
      return nullptr;
  }

  return i->second;
}

//==============================================================================
void type_info::initialize()
{
  for (auto i : db())
  {
    if (i.second->base_info() != nullptr)
    {
      i.second->base_info()->has_derived_ = true;
    }
  }
}

//==============================================================================
bool type_info::is_kind_of(const type_info* typeInfo) const
{
  const type_info* tempTypeInfo = this;

  while (tempTypeInfo != nullptr)
  {
    if (tempTypeInfo == typeInfo)
    {
      return true;
    }

    if (tempTypeInfo == tempTypeInfo->base())
    {
      break;
    }

    tempTypeInfo = tempTypeInfo->base();
  }
  return false;
}

//==============================================================================
bool type_info::is_kind_of(const char* typeName) const
{
  return is_kind_of(type_info::GetTypeInfo(typeName));
}

unsigned type_info::GetInheritanceDepth() const
{
  unsigned r = 0;
  const type_info* typeInfo = this;

  while (typeInfo != nullptr
         && typeInfo != typeInfo->base())
  {
    typeInfo = typeInfo->base();
    r++;
  }

  return r;
}

//==============================================================================
const type_info::db_type& type_info::db()
{
  return db_();
}

//==============================================================================
//==============================================================================
type_info::db_type& type_info::db_()
{
  static type_info::db_type db;
  return db;
}
template <typename T, typename F>
inline T* cast(F* from, const char* typeName)
{
  if (from != nullptr)
  {
    return from->type_info_run_time()->cast<T>(from, typeName);
  }
  else
  {
    return nullptr;
  }
}

} // namespace mflect
