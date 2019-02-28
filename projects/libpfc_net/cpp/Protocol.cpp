#include "sustain/framework/Protocol.h"

#include <sstream>
#include <string>

namespace pfc {

pfc_service_registry_request::~pfc_service_registry_request()
{
  _name.resize(0);
  _address.resize(0);
  _brief.resize(0);
};
//-----------------------------------------------------------------------------
size_t pfc_service_registry_request::Length() const
{
  size_t length = size_of_pfc_type<
    decltype(_message_type)
    //, decltype(_port)
    //, decltype(_protacol)
    //, decltype(_name)
    //, decltype(_address)
    //, decltype(_brief)
    >(_message_type); //,  _port,  _protacol,  _name,  _address,  _brief);
  return length;
}
//-----------------------------------------------------------------------------
pfc_uint pfc_service_registry_request::Type() const
{
  return _message_type;
}
//-----------------------------------------------------------------------------
Error pfc_service_registry_request::serialize(std::ostream& is) const
{

  return serialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(is, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
Error pfc_service_registry_request::deserialize(std::istream& os)
{
  return deserialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(os, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
size_t pfc_service_registry_response::Length() const { return sizeof(_message_type); }
//-----------------------------------------------------------------------------
pfc_uint pfc_service_registry_response::Type() const { return _message_type; }
//-----------------------------------------------------------------------------
Error pfc_service_registry_response::serialize(std::ostream& result) const { return Error(); }
//-----------------------------------------------------------------------------
Error pfc_service_registry_response::deserialize(std::istream& data) { return Error(); }
//-----------------------------------------------------------------------------
//!
//!  comparison of mesages
//!
//-----------------------------------------------------------------------
bool operator==(const pfc_service_registry_request& lhs, const pfc_service_registry_request& rhs)
{
  return lhs._message_type == rhs._message_type
    && lhs._protacol == rhs._protacol
    && lhs._port == rhs._port
    && lhs._name == rhs._name
    && lhs._address == rhs._address
    && lhs._brief == rhs._brief;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_service_registry_request& lhs, const pfc_service_registry_request& rhs)
{
  return !(lhs == rhs);
}
//-----------------------------------------------------------------------
bool operator==(const pfc_service_registry_response& lhs, const pfc_service_registry_response& rhs)
{
  return lhs._message_type == rhs._message_type;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_service_registry_response& lhs, const pfc_service_registry_response& rhs)
{
  return !(lhs == rhs);
}
//-----------------------------------------------------------------------------
//!
//!  size_of_pfc_types
//!
inline size_t size_of_pfc_type(const pfc_byte& result)
{
  return sizeof(pfc_byte);
}
//-----------------------------------------------------------------------------
inline size_t size_of_pfc_type(const pfc_char& result)
{
  return sizeof(pfc_char);
}
//-----------------------------------------------------------------------------
inline size_t size_of_pfc_type(const pfc_uint& result)
{
  return sizeof(pfc_uint);
}
//-----------------------------------------------------------------------------
inline size_t size_of_pfc_type(const pfc_int& result)
{
  return sizeof(pfc_int);
}
//-----------------------------------------------------------------------------
inline size_t size_of_pfc_type(const pfc_float& result)
{
  return sizeof(pfc_float);
}
//-----------------------------------------------------------------------------
inline size_t size_of_pfc_type(const pfc_double& result)
{
  return sizeof(pfc_double);
}
//-----------------------------------------------------------------------------
inline size_t size_of_pfc_type(const pfc_string& result)
{
  return result.size();
}
//-------------------------------------------------------------------------------
//!
//! Serilization of pfc_types
//!
Error serialize_pfc_type(std::ostream& os, const pfc_byte& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_char& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_ushort& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_short& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_uint& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_int& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_float& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_double& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_string& data)
{
  auto size = data.size();
  os.write(reinterpret_cast<const char*>(&size), sizeof(size));
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-----------------------------------------------------------------------------
Error serialize_pfc_type(std::ostream& os, const pfc_protocol& data)
{
  os.write(reinterpret_cast<const char*>(&data), sizeof(data));
  if (os.good()) {
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
//!
//! Deserialization of pfc_types
//!
Error deserialize_pfc_type(std::istream& is, pfc_byte& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_char& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-----------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_ushort& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_short& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_uint& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-----------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_int& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_float& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_double& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_string& result)
{
  //TODO:Fix \0 contained in original string;
  auto size = result.size();
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    result.resize(size);
    if (is.good()) {
      is.read(result.data(), sizeof(size));
    } else {
     return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
    }
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-----------------------------------------------------------------------------
Error deserialize_pfc_type(std::istream& is, pfc_protocol& result)
{
  if (is.good()) {
    is.read(reinterpret_cast<char*>(&result), sizeof(result));
    return Error();
  } else {
    return Error(Error::Code::PFC_IP_SERIALIZATION_ERROR);
  }
}
//-------------------------------------------------------------------------------
}