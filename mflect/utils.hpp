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
#include <sstream>
#include <type_traits>

namespace mflect
{

/**
 * @brief The is_integral struct
 *
 * C++11 STL already has std::is_integral however this one is a bit different.
 * mflect::is_integral<T>::value tells mflect serialization system if type T
 * could be represented as a string. For example we may have:
 *
 * @code
 * struct Vector2
 * {
 * public:
 *   float x;
 *   float y;
 * };
 *
 * MFLECT_DECLARE_TYPE_INTEGRAL(Vector2)
 * @endcode
 *
 * now serialization code can possibly assume that there is Vector2 to string
 * and string to Vector2 conversion specializations.
 */
template <typename T>
struct is_integral
{
  enum { value = false };
};

#define MFLECT_DECLARE_TYPE_INTEGRAL(T) \
  template <> \
  struct is_integral<T> \
  { \
    enum { value = true, }; \
  }; \

// predefined as integral
MFLECT_DECLARE_TYPE_INTEGRAL(bool)
MFLECT_DECLARE_TYPE_INTEGRAL(char)
MFLECT_DECLARE_TYPE_INTEGRAL(unsigned char)
MFLECT_DECLARE_TYPE_INTEGRAL(int)
MFLECT_DECLARE_TYPE_INTEGRAL(long)
MFLECT_DECLARE_TYPE_INTEGRAL(long long)
MFLECT_DECLARE_TYPE_INTEGRAL(unsigned long)
MFLECT_DECLARE_TYPE_INTEGRAL(unsigned long long)
MFLECT_DECLARE_TYPE_INTEGRAL(float)
MFLECT_DECLARE_TYPE_INTEGRAL(double)
MFLECT_DECLARE_TYPE_INTEGRAL(std::string)

//==============================================================================
template<typename T, int A = std::is_abstract<T>::value>
struct make
{
  static T* instance()
  {
    return new T();
  }
};

template<typename T>
struct make<T, 1>
{
  static T* instance()
  {
    MFLECT_RUNTIME_ERROR("can not instantiate an abstract class");
  }
};

//==============================================================================
template<typename T>
struct identity
{
    typedef T type;
};

/**
* VariantConvert - contains implementations of functions which convert strings to arbitrary plain type, supported by stringstream.
*/

namespace VariantConvert
{
    template<typename T>
    inline T from_string_impl(const std::string &s, identity<T>)
    {
        T result;
        std::stringstream ss;
        ss >> std::noskipws;
        ss.str(s);
        ss >> result;
        return result;
    }

    inline bool from_string_impl(const std::string &s, identity<bool>)
    {
        // ios::boolalpha - have you looked for it, while trying to reinvent the wheel?
        // it doesn't suit our needs.. i don't remember why, though..
        if (s == "false" || s.empty())
            return false;

        long double result;
        std::stringstream ss;
        ss.str(s);
        ss >> result;

        if (ss.fail())
            return true;

        return ((int) result != 0);
    }

    inline std::string from_string_impl(const std::string &s, identity<std::string>)
    {
        return s;
    }

    template <typename T>
    inline std::string to_string_impl(const T& t)
    {
            std::ostringstream s;
            s << t;
            return s.str();
    }

    template <>
    inline std::string to_string_impl(const bool& t)
    {
        return t ? "true" : "false";
    }

}    //    namespace VariantConvert

template<typename T>
T from_string(const std::string &s)
{
    return VariantConvert::from_string_impl(s, identity<T>());
}

template<typename T>
std::string to_string(const T& t)
{
    return VariantConvert::to_string_impl(t);
}
//==============================================================================
template <typename From, typename To>
class is_convertible
{
public:
    enum { value = 0 };
};

#define MFLECT_DECLARE_CONVERTIBLE(FROM, TO) \
  template <> \
  class is_convertible<FROM, TO> \
  { \
  public: \
    enum { value = 1 }; \
  }; \

MFLECT_DECLARE_CONVERTIBLE(std::string, std::string)
MFLECT_DECLARE_CONVERTIBLE(std::string, bool)
MFLECT_DECLARE_CONVERTIBLE(std::string, char)
MFLECT_DECLARE_CONVERTIBLE(std::string, unsigned char)
MFLECT_DECLARE_CONVERTIBLE(std::string, int)
MFLECT_DECLARE_CONVERTIBLE(std::string, long)
MFLECT_DECLARE_CONVERTIBLE(std::string, long long)
MFLECT_DECLARE_CONVERTIBLE(std::string, unsigned)
MFLECT_DECLARE_CONVERTIBLE(std::string, unsigned long)
MFLECT_DECLARE_CONVERTIBLE(std::string, unsigned long long)
MFLECT_DECLARE_CONVERTIBLE(std::string, float)
MFLECT_DECLARE_CONVERTIBLE(std::string, double)
MFLECT_DECLARE_CONVERTIBLE(bool, std::string)
MFLECT_DECLARE_CONVERTIBLE(char, std::string)
MFLECT_DECLARE_CONVERTIBLE(unsigned char, std::string)
MFLECT_DECLARE_CONVERTIBLE(int, std::string)
MFLECT_DECLARE_CONVERTIBLE(long, std::string)
MFLECT_DECLARE_CONVERTIBLE(long long, std::string)
MFLECT_DECLARE_CONVERTIBLE(unsigned, std::string)
MFLECT_DECLARE_CONVERTIBLE(unsigned long, std::string)
MFLECT_DECLARE_CONVERTIBLE(unsigned long long, std::string)
MFLECT_DECLARE_CONVERTIBLE(float, std::string)
MFLECT_DECLARE_CONVERTIBLE(double, std::string)

// Conversion from Type to Type for the case, when exception should be
// thrown at runtime when there is no implementation for a conversion.
template<typename T, int A = mflect::is_integral<T>::result>
class Convert
{
public:
    static T FromString(const std::string &s)
    {
        return VariantConvert::from_string_impl(s, identity<T>());
    }

    static std::string ToString(const T &value)
    {
        return VariantConvert::to_string_impl(value);
    }

};

template<typename T>
class Convert<T, 0>
{
public:
    static T FromString(const std::string &s)
    {
        // TODO substitute typename if possible
        MFLECT_RUNTIME_ERROR("Conversion from string to the type %s is not implemented.");
    }

    static std::string ToString(const T &value)
    {
        // TODO substitute typename if possible
        MFLECT_RUNTIME_ERROR("Conversion to string from type %s is not implemented.");
    }

};

//==============================================================================
template <typename Type, int Implemented = is_convertible<std::string, Type>::value>
class TypeInfoStringToTypeHelper
{
public:
    static void StringToType(const std::string&, Type*)
    {
        MFLECT_RUNTIME_ERROR("StringToType conversion not implemented.");
    }

};

template <typename Type>
class TypeInfoStringToTypeHelper<Type, 1>
{
public:
    static void StringToType(const std::string& s, Type* instance)
    {
        *instance = Convert<Type>::FromString(s);
    }

};

template <typename Type, int Implemented = is_convertible<Type, std::string>::value>
class TypeInfoTypeToStringHelper
{
public:
    static std::string TypeToString(Type*)
    {
        MFLECT_RUNTIME_ERROR("TypeToString conversion not implemented.");
    }

};

template <typename Type>
class TypeInfoTypeToStringHelper<Type, 1>
{
public:
    static std::string TypeToString(Type* instance)
    {
        return Convert<Type>::ToString(*instance);
    }

};

} // namespace mflect
