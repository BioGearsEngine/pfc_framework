#ifndef SUSTAIN_PFCNW_PROTOCOL_H
#define SUSTAIN_PFCNW_PROTOCOL_H

#include <sustain/framework/Exports.h>
#include <sustain/framework/Error.h>

#include <cstdint>
#include <cstring>
#include <istream>
#include <vector>
#include <iostream>

using std::uint32_t;

#pragma warning(push)
#pragma warning(disable:4251)
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

enum pfc_protocol : pfc_byte { udp,
                               tcp };

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

struct SUSTAIN_FRAMEWORK_API pfc_service_registry_request : pfc_message {
  pfc_uint _message_type = SERVICE_REGISTRY_REQUEST;
  pfc_ushort _port;
  pfc_protocol _protacol = pfc_protocol::udp;
  pfc_string _name;
  pfc_string _address;
  pfc_string _brief;

  ~pfc_service_registry_request() override;

  size_t Length() const override;
  pfc_uint Type() const override;
  Error serialize(std::ostream& result) const override;
  Error deserialize(std::istream& data) override;
};

struct SUSTAIN_FRAMEWORK_API pfc_service_registry_response : pfc_message {
  pfc_uint _message_type = SERVICE_REGISTRY_RESPONSE;

  size_t Length() const override;
  pfc_uint Type() const override;
  Error serialize(std::ostream& result) const override;
  Error deserialize(std::istream& data) override;
};

//-----------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_service_registry_request&, const pfc_service_registry_request&);
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_service_registry_request&, const pfc_service_registry_request&);
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_service_registry_response&, const pfc_service_registry_response&);
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_service_registry_response&, const pfc_service_registry_response&);
/**
  *!  Serialization functions for ngss types
  **/
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_byte&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_char&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_ushort&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_short&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_uint&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_int&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_float&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_double&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_string&);
Error SUSTAIN_FRAMEWORK_API serialize_pfc_type(std::ostream&, const pfc_protocol&);

template <typename T, size_t N>
Error serialize_pfc_type(std::ostream& is, T (&val)[N])
{
  constexpr size_t size = sizeof(T) * N;
  is.write(reinterpret_cast<const char*>(&val[0]), size - 1);
  return Error();
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

Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_byte& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_char& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_ushort& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_short& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_uint& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_int& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_float& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_double& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_string& result);
Error SUSTAIN_FRAMEWORK_API deserialize_pfc_type(std::istream&, pfc_protocol& result);
template <typename T, int N>
Error deserialize_pfc_type(std::istream& os, T (&result)[N])
{
  constexpr size_t size = sizeof(T) * N;

  os.write(&result[0], size);
  os.write('\0', sizeof('\0'));
  return Error();
}

inline Error deserialize_pfc_type(std::istream& os)
{
  return Error();
}

template <typename T, typename... VAR>
Error deserialize_pfc_type(std::istream& os,
                           T& first, VAR&... args)
{
  auto error = deserialize_pfc_type(os, first);
  error |= deserialize_pfc_type(os, args...);
  return error;
}

/**
  *!  sizeof for pfc_types
  *!  Most just call sizeof, but char[]
  *!  return strlen +/- null terminal
  **/
constexpr size_t size_of_pfc_type(void);
size_t SUSTAIN_FRAMEWORK_API size_of_pfc_type(const pfc_byte& result);
size_t SUSTAIN_FRAMEWORK_API size_of_pfc_type(const pfc_char& result);
size_t SUSTAIN_FRAMEWORK_API size_of_pfc_type(const pfc_uint& result);
size_t SUSTAIN_FRAMEWORK_API size_of_pfc_type(const pfc_int& result);
size_t SUSTAIN_FRAMEWORK_API size_of_pfc_type(const pfc_float& result);
size_t SUSTAIN_FRAMEWORK_API size_of_pfc_type(const pfc_double& result);
size_t SUSTAIN_FRAMEWORK_API size_of_pfc_type(const std::vector<pfc_byte>& result);

template <typename T, size_t N>
int size_of_pfc_type(const T (&)[N])
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
#pragma warning( pop)
}

#endif //SUSTAIN_PFCNW_PROTOCOL_H
