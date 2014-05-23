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

std::string type_info::GetTypeDescriptionString(const std::string& typeName, unsigned depth)
{
  if (type_info_register()[typeName] == 0)
      return "Type info record for type \"" + typeName + "\" doesn't exist.\n";

  type_info& typeInfo= *type_info_register()[typeName];
  std::string identStr(depth, '\t');
  std::string identStr1 = identStr + "\t";
  std::string identStr2 = identStr + "\t\t";
  std::string identStr3 = identStr + "\t\t\t";
  std::string eolStr = "\n";
  std::string r;

  r = identStr + "Description of type\"" + typeName + "\":" + eolStr + eolStr +
      identStr1 + "name: " + typeInfo.name() + eolStr + eolStr +
      identStr1 + "Properties:" + eolStr;

  for (auto i : typeInfo.properties())
  {
      property_info& p = *(i.second);
      r += identStr2 + "key: \"" + (i.first) + "\"" + eolStr +
           identStr3 + "name: \"" + p.Name() + "\"" + eolStr +
           identStr3 + "type name: \"" + p.TypeName() + "\"" + eolStr +
           identStr3 + "owner name: \"" + p.OwnerName() + "\"" + eolStr +
           identStr3 + "valid flags: ";

      if (p.IsArray())
          r += "array, ";
      if (p.IsPointer())
          r += "pointer, ";
      if (p.Integral())
          r += "integral, ";
      r += eolStr;
  }

  if (typeInfo.base_info() != nullptr
      && typeInfo.base_info() != &typeInfo)
  {
      r += eolStr;
      string baseName = typeInfo.base_info()->name();
      r += identStr1 + "Derived from type: \"" + baseName + "\"" + eolStr;
      r += type_info::GetTypeDescriptionString(baseName, depth + 1);
  }

  return r;
}