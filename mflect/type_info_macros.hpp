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
#define MFLECT_CAST(WHAT, TO)                                                  \
  mflect::cast<TO>(WHAT, TO::type_info_static())                               \

//==============================================================================
#define MFLECT_INTERNAL_TYPE_INFO_DECLARATION_BEGIN_(TYPE)                     \
  class type_info_##TYPE : public mflect::type_info                            \
  {                                                                            \
  public:                                                                      \

//==============================================================================
#define MFLECT_INTERNAL_TYPE_INFO_DECLARATION_BASE_(TYPE)                      \
    type_info_##TYPE()                                                         \
    {                                                                          \
      register_type_info_(#TYPE, this);                                        \
    }                                                                          \
                                                                               \
    virtual void* make_new() const                                             \
    {                                                                          \
      return mflect::make<TYPE>::instance();                                   \
    }                                                                          \
                                                                               \
    virtual void make_delete(void* instance) const                             \
    {                                                                          \
      delete static_cast<TYPE*>(instance);                                     \
    }                                                                          \
                                                                               \
    virtual const char* name() const                                           \
    {                                                                          \
      return #TYPE;                                                            \
    }                                                                          \
                                                                               \
    virtual bool is_integral() const                                           \
    {                                                                          \
        return mflect::is_integral<TYPE>::value;                               \
    }                                                                          \
                                                                               \
    virtual void from_string(void* instance, const std::string& value) const   \
    {                                                                          \
      mflect::TypeInfoStringToTypeHelper<TYPE>::StringToType(value,            \
        static_cast<TYPE*>(instance));                                         \
    }                                                                          \
                                                                               \
    virtual std::string to_string(void* instance) const                        \
    {                                                                          \
      return mflect::TypeInfoTypeToStringHelper<TYPE>::                        \
        TypeToString(static_cast<TYPE*>(instance));                            \
    }                                                                          \
                                                                               \
                                                                               \
    static type_info_##TYPE* instance()                                        \
    {                                                                          \
      static type_info_##TYPE instance_;                                       \
      return &instance_;                                                       \
    }                                                                          \

//==============================================================================
#define MFLECT_INTERNAL_TYPE_INFO_DECLARATION_END_(TYPE)                       \
private:                                                                       \
  static type_info_##TYPE* instance_ptr_;                                      \
};                                                                             \
                                                                               \
type_info_##TYPE* type_info_##TYPE::instance_ptr_                              \
  =  type_info_##TYPE::instance();                                             \
                                                                               \

//==============================================================================
#define MFLECT_TYPE_INFO_DECLARE(TYPE)                                         \
  MFLECT_INTERNAL_TYPE_INFO_DECLARATION_BEGIN_(TYPE)                           \
  MFLECT_INTERNAL_TYPE_INFO_DECLARATION_BASE_(TYPE)                            \
                                                                               \
  virtual type_info* type_info_run_time(const void* /*instance*/) const        \
  {                                                                            \
      return type_info_##TYPE::instance();                                     \
  }                                                                            \
                                                                               \
  MFLECT_INTERNAL_TYPE_INFO_DECLARATION_END_(TYPE)                             \

//==============================================================================
#define MFLECT_TYPE_INFO_DECLARE_DERIVED(TYPE_DERIVED, TYPE_BASE)              \
  MFLECT_INTERNAL_TYPE_INFO_DECLARATION_BEGIN_(TYPE_DERIVED)                   \
  MFLECT_INTERNAL_TYPE_INFO_DECLARATION_BASE_(TYPE_DERIVED)                    \
                                                                               \
  virtual const char* base_name() const                                        \
  {                                                                            \
    return #TYPE_BASE;                                                         \
  }                                                                            \
                                                                               \
  virtual type_info* type_info_run_time(const void* instance) const            \
  {                                                                            \
    return static_cast<const TYPE_DERIVED*>(instance)->type_info_run_time();   \
  }                                                                            \
                                                                               \
  MFLECT_INTERNAL_TYPE_INFO_DECLARATION_END_(TYPE_DERIVED)                     \

//==============================================================================
#define MFLECT_INTERNAL_INJECT_TYPE_INFO_BASE_(TYPE)                           \
  public:                                                                      \
    virtual mflect::type_info* type_info_run_time() const                      \
    {                                                                          \
      static mflect::type_info* typeInfo_                                      \
        = mflect::type_info::find_type_info(#TYPE);                            \
      return typeInfo_;                                                        \
    }                                                                          \
                                                                               \
    inline static mflect::type_info* type_info_static()                        \
    {                                                                          \
      static mflect::type_info* typeInfo_                                      \
        = mflect::type_info::find_type_info(#TYPE);                            \
      return typeInfo_;                                                        \
    }                                                                          \
                                                                               \
    private:                                                                   \

//==============================================================================
#define MFLECT_INJECT_TYPE_INFO(TYPE)                                          \
  MFLECT_INTERNAL_INJECT_TYPE_INFO_BASE_(TYPE)                                 \

//==============================================================================
#define MFLECT_INJECT_TYPE_INFO_DERIVED(TYPE_DERIVED, TYPE_BASE)               \
  public:                                                                      \
    typedef TYPE_BASE base_type;                                               \
                                                                               \
  MFLECT_INTERNAL_INJECT_TYPE_INFO_BASE_(TYPE_DERIVED)

//==============================================================================
#define MFLECT_INJECT_PROPERTY(TYPE, NAME)                                     \
  public:                                                                      \
    TYPE Get##NAME() const { return NAME##_; }                                 \
    void Set##NAME(const TYPE& value) { NAME##_ = value; }                     \
                                                                               \
  private:                                                                     \
    TYPE NAME##_;                                                              \

//==============================================================================
#define MFLECT_INJECT_VECTOR_PROPERTY(TYPE, NAME)                              \
  public:                                                                      \
    TYPE Get##NAME(unsigned index) const { return NAME##_[index]; }            \
    void Push##NAME(const TYPE& value) { NAME##_.push_back(value); }           \
    unsigned Get##NAME##Count() const { return NAME##_.size(); }               \
    void Clear##NAME() { NAME##_.clear(); }                                    \
                                                                               \
  private:                                                                     \
    std::vector<TYPE> NAME##_;                                                 \

//==============================================================================
#define MFLECT_INJECT_VECTOR_PTR_PROPERTY(TYPE, NAME)                          \
  public:                                                                      \
    TYPE* Get##NAME(unsigned index) const { return NAME[index]; }              \
    void Push##NAME(TYPE* value) { NAME.push_back(value); }                    \
    unsigned Get##NAME##Count() const { return NAME.size(); }                  \
    void Clear##NAME() { NAME.clear(); }                                       \
                                                                               \
  private:                                                                     \
    std::vector<TYPE*> NAME;                                                   \

