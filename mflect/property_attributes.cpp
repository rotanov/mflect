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

#include "2de_PropertyAttributes.hpp"

#include "2de_Error.h"

PropertyAttributeDefault::PropertyAttributeDefault()
{
}

std::map<PropertyInfo*, PropertyAttributeDefault*>& PropertyAttributeDefault::GetAttributes_()
{
    static std::map<PropertyInfo*, PropertyAttributeDefault*> attributes_;
    return attributes_;
}

bool PropertyAttributeDefault::Exist(PropertyInfo* propertyInfo)
{
    D2D_ASSERT(propertyInfo != NULL);
    return GetAttributes_().find(propertyInfo) != GetAttributes_().end();
}

PropertyAttributeDefault* PropertyAttributeDefault::Get(PropertyInfo* propertyInfo)
{
    D2D_ASSERT(Exist(propertyInfo));
    return GetAttributes_()[propertyInfo];
}

////
TypeInfoComboBoxHint::TypeInfoComboBoxHint()
{

}

bool TypeInfoComboBoxHint::Exist(TypeInfo *typeInfo)
{
    D2D_ASSERT(typeInfo != NULL);
    return GetAttributes_().find(typeInfo) != GetAttributes_().end();
}

TypeInfoComboBoxHint *TypeInfoComboBoxHint::Get(TypeInfo *typeInfo)
{
    D2D_ASSERT(Exist(typeInfo));
    return GetAttributes_()[typeInfo];
}

std::map<TypeInfo *, TypeInfoComboBoxHint *> &TypeInfoComboBoxHint::GetAttributes_()
{
    std::map<TypeInfo *, TypeInfoComboBoxHint *> attributes_;
    return attributes_;
}
