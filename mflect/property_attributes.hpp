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

#include "defines.hpp"
#include "property_info.hpp"

namespace mflect
{

/**
 * @brief The property_attribute_default class
 */
class property_attribute_default
{
public:
  /**
   * @brief Test if owner's value of this property is equal to it's default value.
   * @param owner - pointer to instance of this property's owner. User must
   * ensure it's actual type.
   * @return true if equal, false otherwise
   */
  virtual bool test(const void* owner) const = 0;

  /**
   * @brief Get default-attribute record for this property_info.
   * @param propertyInfo pointer to property_info instance.
   * @return nullptr if there is no record, pointer to record otherwise.
   */
  inline static property_attribute_default* get(property_info* propertyInfo);

protected:
  typedef std::unordered_map<property_info*, property_attribute_default*> db_type;
  inline static db_type& db_();

};

//==============================================================================
property_attribute_default* property_attribute_default::get(property_info* propertyInfo)
{
  if (propertyInfo == nullptr)
  {
    MFLECT_WARNING("supported property_info is nullptr");
  }

  db_type& db = db_();

  if (db.find(propertyInfo) == db.end())
  {
    return nullptr;
  }
  else
  {
    return db_().at(propertyInfo);
  }
}

//==============================================================================
property_attribute_default::db_type& property_attribute_default::db_()
{
  static db_type db_;
  return db_;
}

} // namespace mflect

//==============================================================================
#define MFLECT_DECLARE_ATTRIBUTE_DEFAULT(OWNER, NAME, VALUE)                   \
  class property_attribute_default_##OWNER##NAME                               \
    : public mflect::property_attribute_default                                \
  {                                                                            \
  public:                                                                      \
    typedef property_info_##OWNER##NAME::value_type value_type;                \
                                                                               \
    property_attribute_default_##OWNER##NAME()                                 \
      : defaultValue_(VALUE)                                                   \
    {                                                                          \
      propertyInfo_ = property_info_##OWNER##NAME::instance();                 \
      db_()[propertyInfo_] = this;                                             \
    }                                                                          \
                                                                               \
    virtual bool test(const void* owner) const                                 \
    {                                                                          \
      value_type value;                                                        \
      void* ptr = &value;                                                      \
      propertyInfo_->GetValue(owner, ptr);                                     \
      return value == defaultValue_;                                           \
    }                                                                          \
                                                                               \
  private:                                                                     \
    mflect::property_info* propertyInfo_;                                      \
    static property_attribute_default_##OWNER##NAME instance_;                 \
    value_type defaultValue_;                                                  \
  };                                                                           \
                                                                               \
  property_attribute_default_##OWNER##NAME                                     \
    property_attribute_default_##OWNER##NAME::instance_;                       \

