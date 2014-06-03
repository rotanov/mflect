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

//==============================================================================
#define MFLECT_INTERNAL_PROPERTY_DECLARATION_BEGIN_(OWNER, NAME)               \
  class property_info_##OWNER##NAME : public mflect::property_info             \
  {                                                                            \
  public:                                                                      \

//==============================================================================
#define MFLECT_INTERNAL_PROPERTY_DECLARATION_BASE_(OWNER, TYPE, NAME, FLAGS)   \
    typedef TYPE value_type;                                                   \
    typedef OWNER owner_type;                                                  \
                                                                               \
    property_info_##OWNER##NAME()                                              \
    {                                                                          \
        type_info_properties_(type_info_##OWNER::instance())[#NAME] = this;    \
        flags_ = FLAGS;                                                        \
    }                                                                          \
                                                                               \
    virtual const char* name() const                                           \
    {                                                                          \
      return #NAME;                                                            \
    }                                                                          \
                                                                               \
    virtual const char* type_name() const                                      \
    {                                                                          \
      return #TYPE;                                                            \
    }                                                                          \
                                                                               \
    virtual const char* owner_type_name() const                                \
    {                                                                          \
      return #OWNER;                                                           \
    }                                                                          \
                                                                               \
    static property_info_##OWNER##NAME* instance()                             \
    {                                                                          \
      static property_info_##OWNER##NAME instance_;                            \
      return &instance_;                                                       \
    }                                                                          \

//==============================================================================
#define MFLECT_INTERNAL_PROPERTY_DECLARATION_END_(OWNER, NAME)                 \
  private:                                                                     \
    static property_info_##OWNER##NAME* instance_ptr_;                         \
  };                                                                           \
                                                                               \
  property_info_##OWNER##NAME* property_info_##OWNER##NAME::instance_ptr_      \
    = property_info_##OWNER##NAME::instance();                                 \

//==============================================================================
#define MFLECT_DECLARE_PROPERTY_INFO_EX(OWNER, TYPE, NAME, SETTER, GETTER)     \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BEGIN_(OWNER, NAME)                   \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BASE_(OWNER, TYPE, NAME, 0x00)        \
                                                                               \
    virtual void set_value(void* owner, const void* value) const               \
    {                                                                          \
      OWNER *typedOwner = static_cast<OWNER*>(owner);                          \
      const TYPE* typedProperty = static_cast<const TYPE*>(value);             \
      typedOwner->SETTER(*typedProperty);                                      \
    }                                                                          \
                                                                               \
    virtual void get_value(const void* owner, void*& value) const              \
    {                                                                          \
      *static_cast<TYPE*>(value) = static_cast<const OWNER*>(owner)->GETTER(); \
    }                                                                          \
                                                                               \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_END_(OWNER, NAME)                     \

//==============================================================================
#define MFLECT_DECLARE_PROPERTY_INFO_INPLACE(OWNER, TYPE, NAME, SETTER, GETTER)\
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BEGIN_(OWNER, NAME)                   \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BASE_(OWNER, TYPE, NAME,              \
      mflect::pflag::inplace & mflect::pflag::identity_mask)                   \
                                                                               \
    virtual void set_value(void* owner, const void *value) const               \
    {                                                                          \
      OWNER* typedOwner = static_cast<OWNER*>(owner);                          \
      const TYPE* typedProperty = static_cast<const TYPE*>(value);             \
      typedOwner->SETTER(*typedProperty);                                      \
    }                                                                          \
                                                                               \
    virtual void get_value_inplace(void* owner, void*& value) const            \
    {                                                                          \
      value = &static_cast<OWNER*>(owner)->GETTER();                           \
    }                                                                          \
                                                                               \
    virtual void get_value(const void* owner, void*& value) const              \
    {                                                                          \
      *static_cast<TYPE*>(value) = static_cast<const OWNER*>(owner)->GETTER(); \
    }                                                                          \
                                                                               \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_END_(OWNER, NAME)                     \

//==============================================================================
#define MFLECT_DECLARE_ARRAY_PROPERTY_INFO_EX(OWNER, TYPE, NAME, PUSHER,       \
  GETTER, SIZEGETTER, CLEARER)                                                 \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BEGIN_(OWNER, NAME)                   \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BASE_(OWNER, TYPE, NAME,              \
      mflect::pflag::array & mflect::pflag::identity_mask)                     \
                                                                               \
    virtual void push_value(void* owner, void* value) const                    \
    {                                                                          \
      static_cast<OWNER*>(owner)->PUSHER(*(static_cast<TYPE*>(value)));        \
    }                                                                          \
                                                                               \
    virtual unsigned array_size(const void* owner) const                       \
    {                                                                          \
      return static_cast<const OWNER*>(owner)->SIZEGETTER();                   \
    }                                                                          \
                                                                               \
    virtual void get_value(const void* owner, void*& value, unsigned index) const\
    {                                                                          \
      *static_cast<TYPE*>(value) =                                             \
        static_cast<const OWNER*>(owner)->GETTER(index);                       \
    }                                                                          \
                                                                               \
    virtual void clear(void* owner) const                                      \
    {                                                                          \
      static_cast<OWNER*>(owner)->CLEARER();                                   \
    }                                                                          \
                                                                               \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_END_(OWNER, NAME)                     \

//==============================================================================
#define MFLECT_DECLARE_PTR_ARRAY_PROPERTY_INFO_EX(OWNER, TYPE, NAME, PUSHER,   \
  GETTER, SIZEGETTER, CLEARER)                                                 \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BEGIN_(OWNER, NAME)                   \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BASE_(OWNER, TYPE, NAME,              \
      mflect::pflag::array | mflect::pflag::pointer)                           \
                                                                               \
    virtual void push_value(void* owner, void* value) const                    \
    {                                                                          \
      static_cast<OWNER*>(owner)->PUSHER(static_cast<TYPE*>(value));           \
    }                                                                          \
                                                                               \
    virtual unsigned array_size(const void *owner) const                       \
    {                                                                          \
      return static_cast<const OWNER*>(owner)->SIZEGETTER();                   \
    }                                                                          \
                                                                               \
    virtual void get_value(const void* owner, void*& value, unsigned index) const\
    {                                                                          \
      value = static_cast<const OWNER*>(owner)->GETTER(index);                 \
    }                                                                          \
                                                                               \
    virtual void clear(void* owner) const                                      \
    {                                                                          \
      static_cast<OWNER*>(owner)->CLEARER();                                   \
    }                                                                          \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_END_(OWNER, NAME)                     \

//==============================================================================
#define MFLECT_DECLARE_PTR_PROPERTY_INFO_EX(OWNER, TYPE, NAME, SETTER, GETTER) \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BEGIN_(OWNER, NAME)                   \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_BASE_(OWNER, TYPE, NAME,              \
      mflect::pflag::pointer & mflect::pflag::identity_mask)                   \
                                                                               \
    virtual void set_value(void* owner, const void* value) const               \
    {                                                                          \
      OWNER* typedOwner = static_cast<OWNER*>(owner);                          \
      const TYPE* typedProperty = static_cast<const TYPE*>(value);             \
      typedOwner->SETTER(typedProperty);                                       \
    }                                                                          \
                                                                               \
    virtual void get_value(const void *owner, void*& value) const              \
    {                                                                          \
      value = static_cast<const OWNER*>(owner)->GETTER();                      \
    }                                                                          \
                                                                               \
    MFLECT_INTERNAL_PROPERTY_DECLARATION_END_(OWNER, NAME)                     \

//==============================================================================
#define MFLECT_DECLARE_PTR_PROPERTY_INFO(OWNER, TYPE, NAME) \
  MFLECT_DECLARE_PTR_PROPERTY_INFO_EX(OWNER, TYPE, NAME, Set##NAME, Get##NAME) \

//==============================================================================
#define MFLECT_DECLARE_PROPERTY_INFO(OWNER, TYPE, NAME) \
  MFLECT_DECLARE_PROPERTY_INFO_EX(OWNER, TYPE, NAME, Set##NAME, Get##NAME) \


