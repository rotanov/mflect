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

#include "defines.hpp"

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
    , baseTypeInfo_(nullptr)
    , typeId_(-1)
  {
  }

  virtual ~type_info()
  {
    delete [] cast_table_();
    cast_table_() = nullptr;
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
   * @brief make_delete
   * @param instance
   */
  virtual void make_delete(void* instance) const = 0;

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
   * @brief base_name
   * @return
   */
  inline virtual const char* base_name() const;

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
   * @brief cast
   * @param instance
   * @param typeInfo
   * @return
   */
  template <typename T>
  T* cast(void* instance, type_info* typeInfo);

  /**
   * @brief type_info_register
   * @return
   */
  inline static const db_type& db();

protected:
  /**
   * @brief register_type_info_
   * @param typeName
   * @param typeInfo
   */
  inline void register_type_info_(const std::string& typeName, type_info* typeInfo);

  inline static db_type& db_();

  inline static int*& cast_table_();

  inline static int& type_count_();

  int typeId_;
  bool hasDerived_;
  type_info* baseTypeInfo_;

private:

};

//==============================================================================
template <typename T>
T* type_info::cast(void* instance, type_info* typeInfo)
{
  type_info* typeInfoFrom = type_info_run_time(instance);
  MFLECT_ASSERT(typeInfoFrom != nullptr);
  MFLECT_ASSERT(typeInfo != nullptr);

  if (cast_table_()[typeInfoFrom->typeId_ * type_count_() + typeInfo->typeId_])
  {
    return static_cast<T*>(instance);
  }
  else
  {
    return nullptr;
  }
}

//==============================================================================
type_info* type_info::base() const
{
  return baseTypeInfo_;
}

//==============================================================================
const char* type_info::base_name() const
{
  return "";
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
    auto typeInfo = i.second;
    std::string baseName = typeInfo->base_name();
    if (!baseName.empty())
    {
      if (db().find(baseName) == db().end())
      {
        MFLECT_RUNTIME_ERROR("no type_info record for base type: " + baseName);
      }
      typeInfo->baseTypeInfo_ = db().at(baseName);
      i.second->base()->hasDerived_ = true;
    }
  }

  auto& castTable = cast_table_();
  int typeCount = type_count_();
  castTable = new int [typeCount * typeCount];

  for (auto i : db())
  {
    for (auto j : db())
    {
      castTable[i.second->typeId_ * type_count_() + j.second->typeId_] =
        i.second->is_kind_of(j.second);
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
void type_info::register_type_info_(const std::string& typeName, type_info* typeInfo)
{
  auto& db = type_info::db_();
  if (db.find(typeName) != db.end())
  {
    MFLECT_RUNTIME_ERROR("type_info record for type " + typeName + " has already been registered");
    return;
  }
  db[typeName] = typeInfo;
  typeId_ = type_count_();
  type_count_()++;
}

//==============================================================================
type_info::db_type& type_info::db_()
{
  static type_info::db_type db;
  return db;
}

//==============================================================================
int*& type_info::cast_table_()
{
  static int* cast_table_ = nullptr;
  return cast_table_;
}

//==============================================================================
int& type_info::type_count_()
{
  static int type_count_ = 0;
  return type_count_;
}

//==============================================================================
template <typename T, typename F>
inline T* cast(F* from, type_info* typeInfo)
{
  if (from != nullptr)
  {
    return from->type_info_run_time()->cast<T>(from, typeInfo);
  }
  else
  {
    return nullptr;
  }
}
} // namespace mflect
