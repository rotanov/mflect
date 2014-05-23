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

#include "property_info.hpp"

namespace mflect
{

/**
 * @brief The property_attribute_default class
 */
class property_attribute_default
{
public:
    property_attribute_default();

    /**
     * @brief test
     * @param owner
     * @return
     */
    virtual bool test(const void* owner) const = 0;

    /**
     * @brief exist
     * @param propertyInfo
     * @return
     */
    static bool exist(property_info* propertyInfo);

    /**
     * @brief get
     * @param propertyInfo
     * @return
     */
    static property_attribute_default* get(property_info* propertyInfo);

protected:
    static std::unordered_map<property_info*, property_attribute_default*>& attribute_register_();
};

} // namespace mflect

#define MFLECT_DECLARE_ATTRIBUTE_DEFAULT(OWNER, NAME, VALUE) \
class property_attribute_default_##OWNER##NAME : public property_attribute_default \
{ \
    public: \
        typedef property_info_##OWNER##NAME::ValueType ValueType;\
\
        property_attribute_default_##OWNER##NAME() \
        { \
            propertyInfo_ = PropertyInfo##OWNER##NAME::GetInstance(); \
            attribute_register_()[propertyInfo_] = &instance_; \
        } \
\
        virtual bool Test(const void* owner) const\
        { \
            ValueType value; \
            void* ptr = &value; \
            propertyInfo_->GetValue(owner, ptr); \
            return value == VALUE; \
        } \
\
    private: \
        PropertyInfo* propertyInfo_; \
        static property_attribute_default_##OWNER##NAME instance_; \
}; \
property_attribute_default_##OWNER##NAME property_attribute_default_##OWNER_##NAME::instance_; \

