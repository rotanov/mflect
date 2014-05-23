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

namespace mflect
{

class property_info
{
public:
  virtual const char* name() const = 0;
  virtual const char* type_name() const = 0;
  virtual const char* owner_type_name() const = 0;

  virtual bool IsPointer() const = 0;
  virtual bool Integral() const = 0;
  virtual bool IsArray() const = 0;

  virtual void SetValue(void *owner, const void *property) const = 0;
  virtual void GetValue(const void* owner, void*& value) const = 0;
  virtual void PushValue(void* owner, void* value) const = 0;
  virtual void GetValue(const void* owner, void*& value, unsigned index) const = 0;

  virtual unsigned GetArraySize(const void* owner) const = 0;
  virtual void Clear(void* owner) const = 0;
};

} // namespace mflect

////////////////////////////////////////////////////////////////////////////////
#define MFLECT_BEGIN_DECLARE_PROPERTY_(OWNER, NAME)\
    class PropertyInfo##OWNER##NAME : public PropertyInfo\
    {\
    public:\

////////////////////////////////////////////////////////////////////////////////
#define MFLECT_DECLARE_PROPERTY_BASE_(OWNER, TYPE, NAME)\
        typedef TYPE ValueType; \
\
        PropertyInfo##OWNER##NAME()\
        {\
            TypeInfo::GetTypeInfo(#OWNER)->Properties()[#NAME] = GetInstance();\
        }\
\
        virtual const char* name() const\
        {\
            return #NAME;\
        }\
\
        virtual const char* type_name() const\
        {\
            return #TYPE;\
        }\
\
        virtual const char* owner_type_name() const\
        {\
            return #OWNER;\
        }\
\
        static PropertyInfo##OWNER##NAME* GetInstance();\

////////////////////////////////////////////////////////////////////////////////
#define MFLECT_END_DECLARE_PROPERTY_(OWNER, NAME)\
        static PropertyInfo##OWNER##NAME instance_;\
    };\
\
    PropertyInfo##OWNER##NAME PropertyInfo##OWNER##NAME::instance_;\
\
    PropertyInfo##OWNER##NAME* PropertyInfo##OWNER##NAME::GetInstance()\
    {\
        return &instance_;\
    }\


////////////////////////////////////////////////////////////////////////////////
#define MFLECT_DECLARE_PROPERTY_INFO_EX(OWNER, TYPE, NAME, SETTER, GETTER)\
    MFLECT_BEGIN_DECLARE_PROPERTY_(OWNER, NAME)\
    MFLECT_DECLARE_PROPERTY_BASE_(OWNER, TYPE, NAME)\
\
        virtual void SetValue(void* owner, const void *property) const\
        {\
            OWNER *typedOwner = static_cast<OWNER*>(owner);\
            const TYPE *typedProperty = static_cast<const TYPE*>(property);\
            typedOwner->SETTER(*typedProperty);\
        }\
\
        virtual void GetValue(const void *owner, void*& value) const\
        {\
            *static_cast<TYPE*>(value) = static_cast<const OWNER*>(owner)->GETTER();\
        }\
\
        virtual bool Integral() const\
        {\
            return Deku2D::IsIntegral<TYPE>::result;\
        }\
\
        virtual bool IsPointer() const\
        {\
            return false;\
        }\
\
        virtual bool IsArray() const\
        {\
            return false;\
        }\
\
        virtual void PushValue(void*, void*) const\
        {\
            MFLECT_RUNTIME_ERROR("Not implemented.")\
        }\
\
        virtual unsigned GetArraySize(const void*) const\
        {\
            MFLECT_RUNTIME_ERROR("Not implemented.")\
        }\
\
        virtual void GetValue(const void*, void*&, unsigned) const\
        {\
            MFLECT_RUNTIME_ERROR("Not implemented.")\
        }\
\
        virtual void Clear(void* /*owner*/) const\
        {\
            MFLECT_RUNTIME_ERROR("Not implemented.")\
        }\
\
    MFLECT_END_DECLARE_PROPERTY_(OWNER, NAME)\

////////////////////////////////////////////////////////////////////////////////
#define MFLECT_DECLARE_ARRAY_PROPERTY_INFO_EX(OWNER, TYPE, NAME, PUSHER, GETTER, SIZEGETTER, CLEARER)\
    MFLECT_BEGIN_DECLARE_PROPERTY_(OWNER, NAME)\
    MFLECT_DECLARE_PROPERTY_BASE_(OWNER, TYPE, NAME)\
\
        virtual void SetValue(void*, const void *) const\
        {\
            MFLECT_RUNTIME_ERROR("Not implemented.")\
        }\
\
        virtual void GetValue(const void*, void*&) const\
        {\
            MFLECT_RUNTIME_ERROR("Not implemented.")\
        }\
\
        virtual bool Integral() const\
        {\
            return false;\
        }\
\
        virtual bool IsPointer() const\
        {\
            return false;\
        }\
\
        virtual bool IsArray() const\
        {\
            return true;\
        }\
\
        virtual void PushValue(void *owner, void *value) const\
        {\
            static_cast<OWNER*>(owner)->PUSHER(*(static_cast<TYPE*>(value)));\
        }\
\
        virtual unsigned GetArraySize(const void *owner) const\
        {\
            return static_cast<const OWNER*>(owner)->SIZEGETTER();\
        }\
\
        virtual void GetValue(const void *owner, void*& value, unsigned index) const\
        {\
            *static_cast<TYPE*>(value) = static_cast<const OWNER*>(owner)->GETTER(index);\
        }\
\
        virtual void Clear(void* owner) const\
        {\
            static_cast<OWNER*>(owner)->CLEARER();\
        }\
    MFLECT_END_DECLARE_PROPERTY_(OWNER, NAME)\

////////////////////////////////////////////////////////////////////////////////
#define MFLECT_DECLARE_PTR_ARRAY_PROPERTY_INFO_EX(OWNER, TYPE, NAME, PUSHER, GETTER, SIZEGETTER, CLEARER)\
    MFLECT_BEGIN_DECLARE_PROPERTY_(OWNER, NAME)\
    MFLECT_DECLARE_PROPERTY_BASE_(OWNER, TYPE, NAME)\
\
        virtual void SetValue(void*, const void*) const\
        {\
            MFLECT_RUNTIME_ERROR("Not implemented.")\
        }\
\
        void GetValue(const void*, void*&) const\
        {\
            MFLECT_RUNTIME_ERROR("Not implemented.")\
        };\
\
        virtual bool Integral() const\
        {\
            return false;\
        }\
\
        virtual bool IsPointer() const\
        {\
            return true;\
        }\
\
        virtual bool IsArray() const\
        {\
            return true;\
        }\
\
        virtual void PushValue(void *owner, void *value) const\
        {\
            static_cast<OWNER*>(owner)->PUSHER(static_cast<TYPE*>(value));\
        }\
\
        virtual unsigned GetArraySize(const void *owner) const\
        {\
            return static_cast<const OWNER*>(owner)->SIZEGETTER();\
        }\
\
        virtual void GetValue(const void *owner, void*& value, unsigned index) const\
        {\
            value = static_cast<const OWNER*>(owner)->GETTER(index);\
        }\
\
        virtual void Clear(void* owner) const\
        {\
            static_cast<OWNER*>(owner)->CLEARER();\
        }\
    MFLECT_END_DECLARE_PROPERTY_(OWNER, NAME)\

////////////////////////////////////////////////////////////////////////////////
#define MFLECT_DECLARE_PTR_PROPERTY_INFO_EX(OWNER, TYPE, NAME, SETTER, GETTER)\
    MFLECT_BEGIN_DECLARE_PROPERTY_(OWNER, NAME)\
    MFLECT_DECLARE_PROPERTY_BASE_(OWNER, TYPE, NAME)\
\
    virtual void SetValue(void* owner, const void *property) const\
    {\
        OWNER *typedOwner = static_cast<OWNER*>(owner);\
        const TYPE *typedProperty = static_cast<const TYPE*>(property);\
        typedOwner->SETTER(typedProperty);\
    }\
\
    virtual void GetValue(const void *owner, void*& value) const\
    {\
        value = static_cast<const OWNER*>(owner)->GETTER();\
    }\
\
    virtual bool Integral() const\
    {\
        return Deku2D::IsIntegral<TYPE>::result;\
    }\
\
    virtual bool IsPointer() const\
    {\
        return true;\
    }\
\
    virtual bool IsArray() const\
    {\
        return false;\
    }\
\
    virtual void PushValue(void*, void*) const\
    {\
        MFLECT_RUNTIME_ERROR("Not implemented.")\
    }\
\
    virtual unsigned GetArraySize(const void*) const\
    {\
        MFLECT_RUNTIME_ERROR("Not implemented.")\
    }\
\
    virtual void* GetValue(const void*, void*&, unsigned) const\
    {\
        MFLECT_RUNTIME_ERROR("Not implemented.")\
    }\
\
    virtual void Clear(void* /*owner*/) const\
    {\
        MFLECT_RUNTIME_ERROR("Not implemented.")\
    }\
    MFLECT_END_DECLARE_PROPERTY_(OWNER, NAME)\

////////////////////////////////////////////////////////////////////////////////
#define MFLECT_DECLARE_PTR_PROPERTY_INFO(OWNER, TYPE, NAME)\
    MFLECT_DECLARE_PTR_PROPERTY_INFO_EX(OWNER, TYPE, NAME, Set##NAME, Get##NAME)\

////////////////////////////////////////////////////////////////////////////////
#define MFLECT_DECLARE_PROPERTY_INFO(OWNER, TYPE, NAME)\
    MFLECT_DECLARE_PROPERTY_INFO_EX(OWNER, TYPE, NAME, Set##NAME, Get##NAME)\

