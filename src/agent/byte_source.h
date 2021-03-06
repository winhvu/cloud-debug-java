/**
 * Copyright 2015 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DEVTOOLS_CDBG_DEBUGLETS_JAVA_BYTE_SOURCE_H_
#define DEVTOOLS_CDBG_DEBUGLETS_JAVA_BYTE_SOURCE_H_

#include <endian.h>
#include "common.h"

namespace devtools {
namespace cdbg {

// Reference to immutable buffer with helper methods to read its content. The
// buffer is not owned by this class.
class ByteSource {
 public:
  ByteSource() : ByteSource(nullptr, 0) {}

  explicit ByteSource(const string& s) : ByteSource(&s[0], s.size()) {}

  ByteSource(const void* data, int size)
    : data_(reinterpret_cast<const uint8*>(data)),
      size_(size) {
  }

  ByteSource(const ByteSource& other) = default;

  ByteSource(ByteSource&& other) = default;

  ByteSource& operator= (const ByteSource& other) = default;

  ByteSource& operator= (ByteSource&& other) = default;

  // Direct access to the buffer.
  template <typename T>
  const T* data() const { return reinterpret_cast<const T*>(data_); }

  // Gets the buffer size.
  int size() const { return size_; }

  // Returns true if all the prior "Read" operations were value. Returns false
  // if one of the "Read" functions tried to access invalid location.
  bool is_error() const { return is_error_; }

  // Creates a partial view of the buffer. Trims the range to contain the range
  // within this buffer.
  ByteSource sub(int offset, int size) const {
    if (offset < 0) {
      size += offset;
      offset = 0;
    }

    int end = std::min(size_, offset + size);
    if (offset > end) {
      return ByteSource();
    }

    return ByteSource(data_ + offset, end - offset);
  }

  // Reads signed 8 bit integer from the class file BLOB. Raises error and
  // returns 0 on invalid offset.
  int8 ReadInt8(int offset) {
    return ReadRaw<int8>(offset);
  }

  // Reads unsigned 8 bit integer from the class file BLOB. Raises error and
  // returns 0 on invalid offset.
  uint8 ReadUInt8(int offset) {
    return ReadRaw<uint8>(offset);
  }

  // Reads signed 16 bit integer from the class file BLOB as big-endian.
  // Raises error and returns 0 on invalid offset.
  int16 ReadInt16BE(int offset) {
    return be16toh(ReadRaw<int16>(offset));
  }

  // Reads unsigned 16 bit integer from the class file BLOB as big-endian.
  // Raises error and returns 0 on invalid offset.
  uint16 ReadUInt16BE(int offset) {
    return be16toh(ReadRaw<uint16>(offset));
  }

  // Reads signed 32 bit integer from the class file BLOB as big-endian.
  // Raises error and returns 0 on invalid offset.
  int32 ReadInt32BE(int offset) {
    return be32toh(ReadRaw<int32>(offset));
  }

  // Reads signed 64 bit integer from the class file BLOB as big-endian.
  // Raises error and returns 0 on invalid offset.
  int64 ReadInt64BE(int offset) {
    return be64toh(ReadRaw<int64>(offset));
  }

 private:
  // Reads a value from the class file BLOB. Does not convert between
  // endianness. Raises error and returns 0 if the offset is invalid.
  template <typename T>
  T ReadRaw(int offset) {
    if ((offset < 0) || (offset + sizeof(T) > size_)) {
      DLOG(INFO) << "Bad offset " << offset << " reading ByteSource";
      is_error_ = true;
      return T();
    }

    return *reinterpret_cast<const T*>(data_ + offset);
  }

 private:
  // Pointer to the wrapped buffer. Not owned by this class.
  const uint8* data_;

  // Size of the wrapped buffer in bytes.
  int size_;

  // True if one of the "Read" functions tried to access invalid location.
  bool is_error_ { false };
};

}  // namespace cdbg
}  // namespace devtools


#endif  // DEVTOOLS_CDBG_DEBUGLETS_JAVA_BYTE_SOURCE_H_
