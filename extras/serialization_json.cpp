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

#include <unordered_map>

#include "serialization_json.hpp"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"
#include "rapidjson/stringbuffer.h"

#include "mflect/mflect.hpp"

namespace persistence
{
using namespace rapidjson;
using namespace mflect;

class CStateInfo
{
public:
  std::unordered_map<void*, int> addressTable;
  int ptrCount;
  unsigned depth;
  //            FileStream fileStream;
  StringBuffer stringBuffer;
  PrettyWriter<StringBuffer> writer;
  //            rapidjson::Writer<StringBuffer> writer;

  CStateInfo()
    : ptrCount(0)
    , writer(stringBuffer)
    , depth(0)
  {
  }
};

static void make_json(CStateInfo& state, const void* next, const std::string &nextName)
{
  state.depth++;
  state.writer.StartObject();

  mflect::type_info* typeInfo = mflect::type_info::find_type_info(nextName);
  MFLECT_ASSERT(typeInfo != nullptr);
  bool hadDerived = false;
  while (typeInfo->has_derived())
  {
    if (typeInfo == typeInfo->type_info_run_time(next))
    {
      break;    // TODO: костыль убрать.
    }
    hadDerived = true;
    typeInfo = typeInfo->type_info_run_time(next);
  }

  if (state.depth == 1 || hadDerived)
  {
    state.writer.String("@type");
    state.writer.String(typeInfo->name());
  }

  while (typeInfo)
  {
    auto& props = typeInfo->properties();
    for (auto i : props)
    {
      property_info& p = *i.second;

      const char* propertyName = p.name();
      const char* propertyOwnerName = p.owner_type_name();
      const char* propertyTypeName = p.type_name();

      auto defaultAttr = property_attribute_default::get(&p);
      if (defaultAttr != nullptr)
      {
        if (defaultAttr->test(next))
        {
          continue;
        }
      }

      auto propTypeInfo = mflect::type_info::find_type_info(p.type_name());
      MFLECT_ASSERT(propTypeInfo != nullptr);

      if (p.is_array())
      {
        if (p.array_size(next) == 0)
        {
          continue;
        }

        state.writer.String(p.name());
        state.writer.StartArray();

        if(propTypeInfo->is_integral())
        {
          for (unsigned j = 0; j < p.array_size(next); j++)
          {
            // TODO: missing array of pointers to integrals like std::vector<int*>
            void* value = propTypeInfo->make_new();
            p.get_value(next, value, j);
            state.writer.String(propTypeInfo->to_string(value).c_str());
            propTypeInfo->make_delete(value);
          }
        }
        else
        {
          for (unsigned j = 0; j < p.array_size(next); j++)
          {
            if (p.is_pointer())
            {
              void* value = NULL;
              p.get_value(next, value, j);
              make_json(state, value, propTypeInfo->name());
            }
            else
            {
              void* value = propTypeInfo->make_new();
              p.get_value(next, value, j);
              make_json(state, value, propTypeInfo->name());
              propTypeInfo->make_delete(value);
            }
          }
        }
        state.writer.EndArray();
      }
      else if (propTypeInfo->is_integral())
      {
        if (p.is_pointer())
        {
          void* value = NULL;
          p.get_value(next, value);
          std::string stringValue = propTypeInfo->to_string(value);
          state.writer.String(p.name());
          state.writer.String(stringValue.c_str());
        }
        else
        {
          void* value = propTypeInfo->make_new();
          p.get_value(next, value);
          std::string stringValue = propTypeInfo->to_string(value);
          state.writer.String(p.name());
          state.writer.String(stringValue.c_str());
          propTypeInfo->make_delete(value);
        }
      }
      else
      {
        state.writer.String(p.name());
        bool alreadySerialized = false;
        bool nullPtr = false;
        if (p.is_pointer())
        {
          void* value = NULL;
          p.get_value(next, value);

          if (value == 0)
          {
            nullPtr = true;
          }

          if (nullPtr)
          {
            state.writer.String("0");
          }
          else
          {
            alreadySerialized = state.addressTable.count(value) == 1;
            if (alreadySerialized)
            {
              state.writer.String(("@ptr" + convert<int>::
                                   to_string(state.addressTable[value])).c_str());

            }
            else
            {
              state.ptrCount++;
              state.addressTable[value] = state.ptrCount;
              state.writer.StartObject();
              state.writer.String("@ptr");
              state.writer.String(convert<int>::to_string(state.ptrCount).c_str());
              state.writer.String("@value");
            }
          }
        }

        if (!alreadySerialized && !nullPtr)
        {
          // TODO: seems fishy
          if (p.is_pointer())
          {
            void *value = NULL;
            p.get_value(next, value);
            make_json(state, value, p.type_name());
            state.writer.EndObject();
          }
          else
          {
            void *value = propTypeInfo->make_new();
            p.get_value(next, value);
            make_json(state, value, p.type_name());
            propTypeInfo->make_delete(value);
          }
        }
      }
    }

    if (typeInfo == typeInfo->base())
    {
      break;
    }

    typeInfo = typeInfo->base();
  }
  state.writer.EndObject();
  state.depth--;
}

static void* construct(rapidjson::Document::ValueType* document,
                         const std::string& nextName, void* next)
{
  bool isObject = document->IsObject();
  mflect::type_info* typeInfo = mflect::type_info::find_type_info(nextName);

  for (rapidjson::Document::ValueType::MemberIterator i
         = document->MemberBegin();
       i != document->MemberEnd();
       ++i)
  {
    std::string propertyName = i->name.GetString();

    if (propertyName.length() > 0
        && propertyName[0] == '@')
    {
      if (propertyName == std::string("@type"))
      {
        MFLECT_ASSERT(nextName == i->value.GetString());
      }
      continue;
    }

    auto* prop = typeInfo->find_property(propertyName);

    if (prop == NULL)
    {
      MFLECT_RUNTIME_ERROR("property: " + propertyName
                           + " not found for type: " + typeInfo->name());
    }

    if (i->value.IsObject())
    {
      std::string propTypeName = prop->type_name();
      auto propTypeInfo = mflect::type_info::find_type_info(propTypeName);
      if (propTypeInfo->has_derived())
      {
        for (rapidjson::Document::ValueType::MemberIterator j
               = i->value.MemberBegin();
             j != i->value.MemberEnd();
             ++j)
        {
          if (j->name.GetString() == std::string("@type"))
          {
            propTypeName = j->value.GetString();
            propTypeInfo = mflect::type_info::find_type_info(propTypeName);
          }
        }
      }

      void* value = nullptr;
      if (prop->is_inplace())
      {
        prop->get_value_inplace(next, value);
      }
      else
      {
        value = propTypeInfo->make_new();
      }

      construct(&(i->value), propTypeName, value);

      if (!prop->is_inplace())
      {
        prop->set_value(next, value);
        if (!prop->is_pointer())
        {
          propTypeInfo->make_delete(value);
        }
      }
    }
    else if (i->value.IsArray())
    {
      auto elementInfo = mflect::type_info::find_type_info(prop->type_name());
      if (elementInfo->is_integral())
      {
        for (unsigned j = 0; j < i->value.Size(); j++)
        {
          auto *tempTypeInfo = mflect::type_info::find_type_info(prop->type_name());
          void *temp = tempTypeInfo->make_new();
          tempTypeInfo->from_string(temp, i->value[j].GetString());
          prop->push_value(next, temp);
          tempTypeInfo->make_delete(temp);
        }
      }
      else
      {
        for (unsigned j = 0; j < i->value.Size(); j++)
        {
          std::string typeName = prop->type_name();
          if (mflect::type_info::find_type_info(typeName)->has_derived())
          {
            for (rapidjson::Document::ValueType::MemberIterator k = i->value[j].MemberBegin(); k != i->value[j].MemberEnd(); ++k)
            {
              if (k->name.GetString() == std::string("@type"))
              {
                typeName = k->value.GetString();
              }
            }
          }
          void* value = mflect::type_info::find_type_info(typeName)->make_new();
          construct(&(i->value[j]), typeName, value);
          prop->push_value(next, value);
          if (!prop->is_pointer())
          {
            mflect::type_info::find_type_info(typeName)->make_delete(value);
          }
        }
      }
    }
    else
    {
      auto propTypeInfo = mflect::type_info::find_type_info(prop->type_name());

      void* value = nullptr;
      if (prop->is_inplace())
      {
        prop->get_value_inplace(next, value);
      }
      else
      {
        value = propTypeInfo->make_new();
      }

      propTypeInfo->from_string(value, i->value.GetString());

      if (!prop->is_inplace())
      {
        prop->set_value(next, value);
        if (!prop->is_pointer())
        {
          propTypeInfo->make_delete(value);
        }
      }
    }
  }
  return next;
}

unsigned to_json(const void* instance, const std::string& typeName, char*& buffer)
{
  MFLECT_ASSERT(buffer == nullptr);
  CStateInfo state;
  make_json(state, instance, typeName);
  unsigned bufferSize = state.stringBuffer.GetSize();
  buffer = new char [bufferSize + 1];
  memcpy(buffer, state.stringBuffer.GetString(), bufferSize);
  buffer[bufferSize] = 0;
  return bufferSize;
}

void from_json(void* instance, const std::string& typeName, char* buffer)
{
  rapidjson::Document document;
//  document.Parse<kParseDefaultFlags>(buffer);
  document.ParseInsitu<kParseInsituFlag>(buffer);
  construct(&document, typeName, instance);
}

std::string extract_type_name(char* buffer)
{
  std::string result;
  rapidjson::Document document;
  document.Parse<kParseDefaultFlags>(buffer);
  MFLECT_ASSERT(document.IsObject());

  for (rapidjson::Document::ValueType::MemberIterator i = document.MemberBegin(); i != document.MemberEnd(); ++i)
  {
    std::string propertyName = i->name.GetString();

    if (propertyName.length() > 0 && propertyName[0] == '@')
    {
      if (propertyName == std::string("@type"))
      {
        result = i->value.GetString();
      }
    }
  }
  return result;
}

} // namespace persistence
