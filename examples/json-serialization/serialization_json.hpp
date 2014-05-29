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

namespace persistence
{
/**
 * @brief to_json
 * @param instance
 * @param typeName
 * @param buffer
 * @return strlen() of buffer. Buffer allocation size is strlen(buffer) + 1
 * @todo: Serialize to/from abstract stream, not char buffer. Or not.
 */
unsigned to_json(const void* instance, const std::string& typeName, char*& buffer);

/**
 * @brief from_json
 * @param instance
 * @param typeName
 * @param buffer
 */
void from_json(void* instance, const std::string& typeName, char* buffer);

/**
 * @brief Extracts top level type name assuming buffer is a result of to_json.
 * @param buffer
 * @return
 */
std::string extract_type_name(char* buffer);

} // namespace persistence
