#ifndef SUSTAIN_PFCNW_PROTOCOL_H
#define SUSTAIN_PFCNW_PROTOCOL_H

/**************************************************************************************
copyright 2019 applied research associates, inc.
licensed under the apache license, version 2.0 (the "license"); you may not use
this file except in compliance with the license. you may obtain a copy of the license
at:
http://www.apache.org/licenses/license-2.0
unless required by applicable law or agreed to in writing, software distributed under
the license is distributed on an "as is" basis, without warranties or
conditions of any kind, either express or implied. see the license for the
specific language governing permissions and limitations under the license.
**************************************************************************************/


#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Error.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <istream>
#include <vector>

using std::uint32_t;

#pragma warning(push)
#pragma warning(disable : 4251)
namespace pfc {
//-----------------------------------------------------------------------
//ICD 3. Message Data Types
//-----------------------------------------------------------------------
using pfc_byte = uint8_t;
using pfc_bool = uint8_t;
using pfc_char = char;
using pfc_ushort = uint16_t;
using pfc_short = int16_t;
using pfc_uint = uint32_t;
using pfc_int = int32_t;
using pfc_float = float;
using pfc_double = double;
using pfc_string = std::string;
using pfc_datetime = uint32_t;

enum pfc_protocol : pfc_byte { pub_sub,
                               req_req };

SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_protocol&);
constexpr pfc_bool pfc_true = true;
constexpr pfc_bool pfc_false = false;

struct SUSTAIN_FRAMEWORK_API pfc_message {
  virtual ~pfc_message() {}

  virtual size_t Length() const = 0;
  virtual pfc_uint Type() const = 0;
  virtual Error serialize(std::ostream& is) const = 0;
  virtual Error deserialize(std::istream& os) = 0;
};
//-----------------------------------------------------------------------
//ICD 4. Message Layout.
//-----------------------------------------------------------------------
constexpr pfc_uint MESSAGE_TYPE_NOT_ASSIGNED = 0x00000000;

//-----------------------------------------------------------------------
//ICD 1.0 -- UDP Multicast Messages
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
//ICD 6.1 Get Current Location
//-----------------------------------------------------------------------
constexpr pfc_uint SERVICE_REGISTRY_REQUEST = 0x00000001;
constexpr pfc_uint SERVICE_REGISTRY_RESPONSE = 0x10000001;

struct SUSTAIN_FRAMEWORK_API pfc_service_announcment : pfc_message {
  pfc_uint _message_type = SERVICE_REGISTRY_REQUEST;
  pfc_ushort _port;
  pfc_protocol _protacol = pfc_protocol::pub_sub;
  pfc_string _name;
  pfc_string _address;
  pfc_string _brief;

  ~pfc_service_announcment() override;

  size_t Length() const override;
  pfc_uint Type() const override;
  Error serialize(std::ostream& os) const override;
  Error deserialize(std::istream& is) override;
};
SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_service_announcment&);
//-----------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_service_announcment&, const pfc_service_announcment&);
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_service_announcment&, const pfc_service_announcment&);
/**
  *!  sizeof for pfc_types
  *!  Most just call sizeof, but char[]
  *!  return strlen +/- null terminal
  **/
template <typename T>
size_t size_of_pfc_type(const T& result)
{
  return sizeof(result);
}

template <>
inline size_t size_of_pfc_type(const std::string& result)
{
  return result.length() * sizeof(std::string::value_type);
}

template <typename T, size_t N>
size_t size_of_pfc_type(const T (&)[N])
{
  constexpr size_t size = sizeof(T) * N;
  return size - 1;
}

constexpr size_t size_of_pfc_type()
{
  return 0;
}

template <typename T, typename... VAR>
size_t size_of_pfc_type(const T& first, const VAR&... args)
{
  return size_of_pfc_type(first) + size_of_pfc_type(args...);
}
/**
  *!  Serialization functions for ngss types
  **/
template <typename T>
Error serialize_pfc_type(std::ostream& os, const T& data)
{
  os.write(reinterpret_cast<const char*>(&data), size_of_pfc_type(data));
  if (os.good()) {
    return Success();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}

template <>
inline Error serialize_pfc_type(std::ostream& os, const pfc_string& data)
{
  auto size = size_of_pfc_type(data);
  os.write(reinterpret_cast<const char*>(&size), size_of_pfc_type(size));
  os.write(reinterpret_cast<const char*>(data.data()), size);
  if (os.good()) {
    return Success();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}

template <typename T, size_t N>
Error serialize_pfc_type(std::ostream& is, T (&val)[N])
{
  constexpr size_t size = size_of_pfc_type(T) * N;
  is.write(reinterpret_cast<const char*>(&val[0]), size - 1);
  return Success();
}

template <typename T, typename... VAR>
Error serialize_pfc_type(std::ostream& buffer, const T& first, const VAR&... args)
{
  auto error = serialize_pfc_type(buffer, first);
  return error |= serialize_pfc_type(buffer, args...);
}
/**
  *!  De-serialization functions for ngss types
  *!  begin - [in]  iterator to a const vector.  Assumption that all iterators are valid for the life of the call
  *!  end   - [out] iterator reference to a const vector.  Points to the next unused byte in the vector.
  **/

template <typename T>
Error deserialize_pfc_type(std::istream& is, T& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), size_of_pfc_type(result));
    return Success();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}

template <>
inline Error deserialize_pfc_type(std::istream& is, pfc_string& result)
{
  //TODO:Fix \0 contained in original string;
  auto size = result.size();
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&size), size_of_pfc_type(size));
    result.resize(size);
    if (is.good()) {
      is.read(const_cast<char*>(result.data()), size_of_pfc_type(result));
    } else {
      return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
    }
    return Success();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
template <typename T, int N>
Error deserialize_pfc_type(std::istream& os, T (&result)[N])
{
  constexpr size_t size = size_of_pfc_type(T) * N;

  os.write(&result[0], size);
  os.write('\0', size_of_pfc_type('\0'));
  return Success();
}

inline Error deserialize_pfc_type(std::istream& os)
{
  return Success();
}

template <typename T, typename... VAR>
Error deserialize_pfc_type(std::istream& os,
                           T& first, VAR&... args)
{
  auto error = deserialize_pfc_type(os, first);
  error |= deserialize_pfc_type(os, args...);
  return error;
}
#pragma warning(pop)
}

#endif //SUSTAIN_PFCNW_PROTOCOL_H
