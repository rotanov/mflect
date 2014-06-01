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

#include <string>

#include "defines.hpp"
#include "type_info.hpp"

namespace mflect
{

enum class pflag : unsigned
{
  inplace = 0x01,
  array = 0x02,
  pointer = 0x04,
  identity_mask = 0xffffffff,
};

inline unsigned operator &(const unsigned lhs, const pflag rhs)
  { return lhs & static_cast<unsigned>(rhs); }
inline unsigned operator &(const pflag lhs, const pflag rhs)
  { return static_cast<unsigned>(lhs) & rhs; }
inline unsigned operator |(const unsigned lhs, const pflag rhs)
  { return lhs | static_cast<unsigned>(rhs); }
inline unsigned operator |(const pflag lhs, const pflag rhs)
  { return static_cast<unsigned>(lhs) | rhs; }


class property_info
{
public:
  virtual const char* name() const = 0;
  virtual const char* type_name() const = 0;
  virtual const char* owner_type_name() const = 0;

  inline bool is_pointer() const;
  inline bool is_array() const;

  virtual void SetValue(void *owner, const void *property) const = 0;
  virtual void GetValue(const void* owner, void*& value) const = 0;
  virtual void PushValue(void* owner, void* value) const = 0;
  virtual void GetValue(const void* owner, void*& value, unsigned index) const = 0;

  virtual unsigned GetArraySize(const void* owner) const = 0;
  virtual void Clear(void* owner) const = 0;

protected:
  inline type_info::property_db_type& type_info_properties_(type_info* typeInfo);

  unsigned flags_;
};

//==============================================================================
bool property_info::is_pointer() const
{
  return flags_ & pflag::pointer;
}

//==============================================================================
bool property_info::is_array() const
{
  return flags_ & pflag::array;
}

//==============================================================================
type_info::property_db_type& property_info::type_info_properties_(type_info* typeInfo)
{
  return typeInfo->properties_;
}

} // namespace mflect
