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

#include "2de_Serialization.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"
#include "rapidjson/stringbuffer.h"

#include "2de_TypeInfo.h"
#include "2de_PropertyAttributes.hpp"
#include "2de_Core.h"

#ifdef TARGET_EDITOR
    #include <QDebug>
    #include "gengine.h"
#endif

namespace Deku2D
{
namespace Serialization
{
using namespace rapidjson;

class CStateInfo
{
public:
    std::map<void*, int> addressTable;
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

static void Helper(CStateInfo& state, const void* next, const std::string &nextName)
{
    state.depth++;
    state.writer.StartObject();

    TypeInfo *typeInfo = TypeInfo::GetTypeInfo(nextName);
    bool hadDerived = false;
    while (typeInfo->HasDerived())
    {
        if (typeInfo == typeInfo->GetRunTimeTypeInfo(next))
        {
            break;    // TODO: костыль убрать.
        }
        hadDerived = true;
        typeInfo = typeInfo->GetRunTimeTypeInfo(next);
    }

    if (state.depth == 1 || hadDerived)
    {
        state.writer.String("@type");
        state.writer.String(typeInfo->Name());
    }

    while (typeInfo)
    {
        map<string, PropertyInfo*> &props = typeInfo->Properties();
        for (map<string, PropertyInfo*>::iterator i = props.begin(); i != props.end(); ++i)
        {
            PropertyInfo& p = *i->second;

            const char* propertyName = p.Name();
            const char* propertyOwnerName = p.OwnerName();
            const char* propertyTypeName = p.TypeName();

            if (PropertyAttributeDefault::Exist(&p))
            {
                if (PropertyAttributeDefault::Get(&p)->Test(next))
                {
                    continue;
                }
            }

            if (p.Integral())
            {
                if (p.IsPointer())
                {
                    void* value = NULL;
                    p.GetValue(next, value);
                    std::string stringValue = p.GetTypeInfo()->GetString(value);
                    state.writer.String(p.Name());
                    state.writer.String(stringValue.c_str());
                }
                else
                {
                    void* value = p.GetTypeInfo()->New();
                    p.GetValue(next, value);
                    std::string stringValue = p.GetTypeInfo()->GetString(value);
                    state.writer.String(p.Name());
                    state.writer.String(stringValue.c_str());
                    delete value;
                }
            }
            else if (p.IsArray())
            {
                if (p.GetArraySize(next) == 0)
                {
                    continue;
                }

                state.writer.String(p.Name());
                state.writer.StartArray();

                TypeInfo* elementInfo = p.GetTypeInfo();

                if(elementInfo->IsIntegral())
                {
                    for (int j = 0; j < p.GetArraySize(next); j++)
                    {
                        // TODO: missing array of pointers to integrals like std::vector<int*>
                        void* value = elementInfo->New();
                        p.GetValue(next, value, j);
                        state.writer.String(elementInfo->GetString(value).c_str());
                        delete value;
                    }
                }
                else
                {
                    for (int j = 0; j < p.GetArraySize(next); j++)
                    {
                        if (p.IsPointer())
                        {
                            void* value = NULL;
                            p.GetValue(next, value, j);
                            Helper(state, value, elementInfo->Name());
                        }
                        else
                        {
                            void* value = p.GetTypeInfo()->New();
                            p.GetValue(next, value, j);
                            Helper(state, value, elementInfo->Name());
                            delete value;
                        }
                    }
                }
                state.writer.EndArray();
            }
            else
            {
                state.writer.String(p.Name());
                bool alreadySerialized = false;
                bool nullPtr = false;
                if (p.IsPointer())
                {
                    void* value = NULL;
                    p.GetValue(next, value);

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
                            state.writer.String(("@ptr" + Convert<int>::
                                ToString(state.addressTable[value])).c_str());

                        }
                        else
                        {
                            state.ptrCount++;
                            state.addressTable[value] = state.ptrCount;
                            state.writer.StartObject();
                            state.writer.String("@ptr");
                            state.writer.String(Convert<int>::ToString(state.ptrCount).c_str());
                            state.writer.String("@value");
                        }
                    }
                }

                if (!alreadySerialized && !nullPtr)
                {
                    // TODO: seems fishy
                    if (p.IsPointer())
                    {
                        void *value = NULL;
                        p.GetValue(next, value);
                        Helper(state, value, p.TypeName());
                        state.writer.EndObject();
                    }
                    else
                    {
                        void *value = p.GetTypeInfo()->New();
                        p.GetValue(next, value);
                        Helper(state, value, p.TypeName());
                        delete value;
                    }
                }
            }
        }

        if (typeInfo == typeInfo->BaseInfo())
        {
            break;
        }

        typeInfo = typeInfo->BaseInfo();
    }
    state.writer.EndObject();
    state.depth--;
}

static void* ParseHelper(rapidjson::Document::ValueType* document, const std::string& nextName, void* next)
{
    bool isObject = document->IsObject();
    TypeInfo *typeInfo = TypeInfo::GetTypeInfo(nextName);
    //void *next = typeInfo->New();
    for (rapidjson::Document::ValueType::MemberIterator i = document->MemberBegin(); i != document->MemberEnd(); ++i)
    {
        std::string propertyName = i->name.GetString();

        if (propertyName.length() > 0 && propertyName[0] == '@')
        {
            if (propertyName == string("@type"))
            {
                D2D_ASSERT(nextName == i->value.GetString());
            }
            continue;
        }

        PropertyInfo *prop = typeInfo->FindProperty(propertyName);

        if (prop == NULL)
        {
            D2D_RUNTIME_ERROR("Property: " + propertyName + " not found for type: " + typeInfo->Name());
        }

        if (i->value.IsObject())
        {
            string typeName = prop->TypeName();
            if (TypeInfo::GetTypeInfo(typeName)->HasDerived())
            {
                for (rapidjson::Document::ValueType::MemberIterator j = i->value.MemberBegin(); j != i->value.MemberEnd(); ++j)
                {
                    if (j->name.GetString() == string("@type"))
                    {
                        typeName = j->value.GetString();
                    }
                }
            }

            void* value = TypeInfo::GetTypeInfo(typeName)->New();
            ParseHelper(&(i->value), typeName, value);
            prop->SetValue(next, value);
            if (!prop->IsPointer())
            {
                delete value;
            }
        }
        else if (i->value.IsArray())
        {
            TypeInfo* elementInfo = TypeInfo::GetTypeInfo(prop->TypeName());
            if (elementInfo->IsIntegral())
            {
                for (int j = 0; j < i->value.Size(); j++)
                {
                    TypeInfo *tempTypeInfo = TypeInfo::GetTypeInfo(prop->TypeName());
                    void *temp = tempTypeInfo->New();
                    tempTypeInfo->SetString(temp, i->value[j].GetString());
                    prop->PushValue(next, temp);
                    delete temp;
                }
            }
            else
            {
                for (int j = 0; j < i->value.Size(); j++)
                {
                    string typeName = prop->TypeName();
                    if (TypeInfo::GetTypeInfo(typeName)->HasDerived())
                    {
                        for (rapidjson::Document::ValueType::MemberIterator k = i->value[j].MemberBegin(); k != i->value[j].MemberEnd(); ++k)
                        {
                            if (k->name.GetString() == string("@type"))
                            {
                                typeName = k->value.GetString();
                            }
                        }
                    }
                    void* value = TypeInfo::GetTypeInfo(typeName)->New();
                    ParseHelper(&(i->value[j]), typeName, value);
                    prop->PushValue(next, value);
                    if (!prop->IsPointer())
                    {
                        delete value;
                    }
                }
            }
        }
        else
        {
            TypeInfo *tempTypeInfo = TypeInfo::GetTypeInfo(prop->TypeName());
            void *temp = tempTypeInfo->New();
            tempTypeInfo->SetString(temp, i->value.GetString());
            prop->SetValue(next, temp);
            delete temp;
        }
    }
    return next;
}

unsigned ToJSON(const void* instance, const std::string& typeName, char*& buffer)
{
    D2D_ASSERT(buffer == NULL);
    CStateInfo state;

#ifdef TARGET_EDITOR
//            timerInit();
#endif
    Helper(state, instance, typeName);
#ifdef TARGET_EDITOR
//            qDebug() << "ToJSON took: " << timerGet() << "ms" << " type name: " << typeName.c_str();
#endif

    unsigned bufferSize = state.stringBuffer.Size();
    buffer = new char [bufferSize];
    memcpy(buffer, state.stringBuffer.GetString(), bufferSize);
    return bufferSize;
}

void FromJSON(void* instance, const std::string& typeName, char* buffer)
{
    rapidjson::Document document;
//            document.Parse<kParseDefaultFlags>(buffer);
    document.ParseInsitu<kParseInsituFlag>(buffer);
#ifdef TARGET_EDITOR
//            timerInit();
#endif
    ParseHelper(&document, typeName, instance);
#ifdef TARGET_EDITOR
//            qDebug() << "FromJSON took: " << timerGet() << "ms" << " type name: " << typeName.c_str();
#endif
}

std::string ExtractTopLevelTypeName(char* buffer)
{
    std::string result;
    rapidjson::Document document;
    document.Parse<kParseDefaultFlags>(buffer);
    D2D_ASSERT(document.IsObject());

    for (rapidjson::Document::ValueType::MemberIterator i = document.MemberBegin(); i != document.MemberEnd(); ++i)
    {
        std::string propertyName = i->name.GetString();

        if (propertyName.length() > 0 && propertyName[0] == '@')
        {
            if (propertyName == string("@type"))
            {
                result = i->value.GetString();
            }
        }
    }
    return result;
}

}    // namespace Serialization

}    // namespace Deku2D
