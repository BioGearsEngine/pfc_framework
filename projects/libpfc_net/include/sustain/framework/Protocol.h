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

/*! \file */


/*! \mainpage Drawing Shapes
 *
 * This project is a reference implemnentation of the SUSTAIN Framework specification.
 * It can be used to create pub/sub and req/request style services for a SUSTAIN Simulation labratory
 * For more general information visit <a href="https://SustainTrainer.com">our website</a>
 * - \subpage pfc::Service  "PFC is a Service based framework"
 * - \subpage Protocol.h "Basic building blocks of framework messeging"
 *
 * This implementation depends on nanomsg and boost::asio
 */

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
//
using pfc_byte = uint8_t;                     //!<  PFC Standard definition of a single byte as a unsigned 8 bit element
using pfc_bool = uint8_t;                     //!<  PFC Standard definition of a boolean value  unsigned 8 bit element
using pfc_char = char;                        //!<  PFC Standard definition of a character as a signed 8 bit element
using pfc_ushort = uint16_t;                  //!<  PFC Standard definition of a ushort  as a unsigned 16 bit element
using pfc_short = int16_t;                    //!<  PFC Standard definition of a short  as a signed 16 bit element
using pfc_uint = uint32_t;                    //!<  PFC Standard definition of a uint  as a unsigned 32 bit element
using pfc_int = int32_t;                      //!<  PFC Standard definition of a int  as a signed 32 bit element
using pfc_float = float;                      //!<  PFC Standard definition of a float as a IEEE 754 32-bit float element
using pfc_double = double;                    //!<  PFC Standard definition of a double as a IEEE 754 64-bit element
using pfc_string = std::basic_string<pfc_char>;   //!<  PFC Standard definition of a string as a stl::basic_string<pfc_char>
using pfc_datetime = uint32_t;                //!<  PFC Standard definition of a datetime as a unsigned 32 bit stored as the number of seconds since Jan 1 1970 Unix Style timestamp


//!
//! Enum for determining which pfc_protocol is being used May expand over time do not assume values 
//!
enum pfc_protocol : pfc_byte { pub_sub,
                               req_req };

//!
//!  Operator for seralizing pfc_protocols to ostreams aka toString(pfc_protocol) function
//!
SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_protocol&);

[[deprecated]] constexpr pfc_bool pfc_true = true;        //!< Defines a namespace scoped standard value for true 
[[deprecated]] constexpr pfc_bool pfc_false = false; //!< Defines a namespace scoped standard value for false - Maybe
constexpr pfc_bool True = true;           //!< Defines a namespace scoped standard value for true 
constexpr pfc_bool False = false;        //!< Defines a namespace scoped standard value for false


//!
//! Standard message format of the PFC Networking protocol
//! Abstract interface that is implemented by various protocols
//!
struct SUSTAIN_FRAMEWORK_API pfc_message {
  virtual ~pfc_message() {}

  virtual size_t Length() const = 0;                    //!< Length of the contents of the message
  virtual pfc_uint Type() const = 0;                    //!< Type of message values are implemenation specific ref to documentation.
  virtual Error serialize(std::ostream& is) const = 0;  //!< Interface call of converting derived class to a seralized format and placing it on the given ostream. Error returns are implementation specific
  virtual Error deserialize(std::istream& os) = 0;      //!< Interface call for inflating a derived class from a seralized input. Error returns are implementation specific
};
//-----------------------------------------------------------------------
//ICD 4. Message Layout.
//-----------------------------------------------------------------------
constexpr pfc_uint MESSAGE_TYPE_NOT_ASSIGNED = 0x00000000;       //!<  Default value of Type() for pfc_message for initialization

//-----------------------------------------------------------------------
//ICD 1.0 -- UDP Multicast Messages
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Service Registry Message
//-----------------------------------------------------------------------
//

//-------------------------------------Service Announcement    --------------------------------------------------------------------------
constexpr pfc_uint SERVICE_Announcement_REQUEST = 0x00000001; //!<  value returned by type() from a pfc_service_announcement
constexpr pfc_uint SERVICE_Announcement_RESPONSE = 0x10000001; //!<  value returned by type() from a pfc_service_announcement_response

struct SUSTAIN_FRAMEWORK_API pfc_service_announcement : pfc_message {
  pfc_uint _message_type = SERVICE_Announcement_REQUEST; //!< value returned by TYPE. Shoudld be constant for any specific message type unless a deseralization error has occured.
  pfc_ushort _port;                                     //!< Stores a listinging port of the registered remote service
  pfc_protocol _protacol = pfc_protocol::pub_sub;       //!< Stores teh protacol used byt the registered service
  pfc_string _name;                                     //!< Human Readable Name of the Service                                                        
  pfc_string _address;                                  //!< Valid URI for communicating with the service. Depending on the protcol implementation may be an IP4/IP6 or System Socket
  pfc_string _brief;                                    //!< Human Description of the service and its feature set. 

  ~pfc_service_announcement() override;

  size_t Length() const override;                   //!< Returns the length of the encoded message
  pfc_uint Type() const override;                   //!< Returns teh _message_type of an encoded message
  Error serialize(std::ostream& os) const override; //!< Sends teh seralized format of a pfc_service_announcement over the provided  ostream. 
  Error deserialize(std::istream& is) override;     //!< Marshalls a seralized service announcement and inflates it the data binding.
};
//!< Stream Operator for converting a pfc_service_announcement over an ostream
SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_service_announcement&);
//-----------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------
//! Equivalance Operator for pfc_service_announcements
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_service_announcement&, const pfc_service_announcement&);
//! Implemented as ! operator==()
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_service_announcement&, const pfc_service_announcement&);

//-------------------------------------Service signoff--------------------------------------------------------------------------
constexpr pfc_uint SERVICE_signoff_REQUEST = 0x00000002; //!<  value returned by type() from a pfc_service_signoff
constexpr pfc_uint SERVICE_signoff_RESPONSE = 0x10000002; //!<  value returned by type() from a pfc_service_signoff_response

struct SUSTAIN_FRAMEWORK_API pfc_service_signoff : pfc_message {
  pfc_uint _message_type = SERVICE_signoff_REQUEST; //!< value returned by TYPE. Shoudld be constant for any specific message type unless a deseralization error has occured.
  pfc_ushort _port; //!< Stores a listinging port of the registered remote service
  pfc_protocol _protacol = pfc_protocol::pub_sub; //!< Stores teh protacol used byt the registered service
  pfc_string _name; //!< Human Readable Name of the Service
  pfc_string _address; //!< Valid URI for communicating with the service. Depending on the protcol implementation may be an IP4/IP6 or System Socket
  pfc_string _brief; //!< Human Description of the service and its feature set.

  ~pfc_service_signoff() override;

  size_t Length() const override; //!< Returns the length of the encoded message
  pfc_uint Type() const override; //!< Returns teh _message_type of an encoded message
  Error serialize(std::ostream& os) const override; //!< Sends teh seralized format of a pfc_service_signoff over the provided  ostream.
  Error deserialize(std::istream& is) override; //!< Marshalls a seralized service signoff and inflates it the data binding.
};
//!< Stream Operator for converting a pfc_service_signoff over an ostream
SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_service_signoff&);
//-----------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------
//! Equivalance Operator for pfc_service_signoffs
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_service_signoff&, const pfc_service_signoff&);
//! Implemented as ! operator==()
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_service_signoff&, const pfc_service_signoff&);

//-------------------------------------Service ServiceList--------------------------------------------------------------------------
constexpr pfc_uint REGISTRY_LIST_REQUEST = 0x00000003; //!<  value returned by type() from a pfc_registry_request
constexpr pfc_uint REGISTRY_LIST_RESPONSE = 0x10000003; //!<  value returned by type() from a pfc_registry_request_response

struct SUSTAIN_FRAMEWORK_API pfc_registry_request : pfc_message {
  pfc_uint _message_type = REGISTRY_LIST_REQUEST; //!< value returned by TYPE. Shoudld be constant for any specific message type unless a deseralization error has occured.
  pfc_ushort _port; //!< Stores a listinging port of the registered remote service
  pfc_protocol _protacol = pfc_protocol::pub_sub; //!< Stores teh protacol used byt the registered service
  pfc_string _name; //!< Human Readable Name of the Service
  pfc_string _address; //!< Valid URI for communicating with the service. Depending on the protcol implementation may be an IP4/IP6 or System Socket
  pfc_string _brief; //!< Human Description of the service and its feature set.

  ~pfc_registry_request() override;

  size_t Length() const override; //!< Returns the length of the encoded message
  pfc_uint Type() const override; //!< Returns teh _message_type of an encoded message
  Error serialize(std::ostream& os) const override; //!< Sends teh seralized format of a pfc_registry_request over the provided  ostream.
  Error deserialize(std::istream& is) override; //!< Marshalls a seralized service ServiceList and inflates it the data binding.
};
//!< Stream Operator for converting a pfc_registry_request over an ostream
SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_registry_request&);
//-----------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------
//! Equivalance Operator for pfc_registry_requests
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_registry_request&, const pfc_registry_request&);
//! Implemented as ! operator==()
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_registry_request&, const pfc_registry_request&);


struct SUSTAIN_FRAMEWORK_API pfc_registry_response : pfc_message {
  pfc_uint _message_type = REGISTRY_LIST_RESPONSE; //!< value returned by TYPE. Shoudld be constant for any specific message type unless a deseralization error has occured.
  pfc_ushort _port; //!< Stores a listinging port of the registered remote service
  pfc_protocol _protacol = pfc_protocol::pub_sub; //!< Stores teh protacol used byt the registered service
  pfc_string _name; //!< Human Readable Name of the Service
  pfc_string _address; //!< Valid URI for communicating with the service. Depending on the protcol implementation may be an IP4/IP6 or System Socket
  pfc_string _brief; //!< Human Description of the service and its feature set.

  ~pfc_registry_response() override;

  size_t Length() const override; //!< Returns the length of the encoded message
  pfc_uint Type() const override; //!< Returns teh _message_type of an encoded message
  Error serialize(std::ostream& os) const override; //!< Sends teh seralized format of a pfc_registry_response over the provided  ostream.
  Error deserialize(std::istream& is) override; //!< Marshalls a seralized service ServiceList and inflates it the data binding.
};
//!< Stream Operator for converting a pfc_registry_response over an ostream
SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_registry_response&);
//-----------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------
//! Equivalance Operator for pfc_registry_responses
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_registry_response&, const pfc_registry_response&);
//! Implemented as ! operator==()
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_registry_response&, const pfc_registry_response&);

//-------------------------------------Service ServiceList----------------------------------------//-------------------------------------Service ServiceList--------------------------------------------------------------------------
constexpr pfc_uint CONNECTION_HERTBEAT_REQUEST = 0x00000004; //!<  value returned by type() from a pfc_heartbeat_request
constexpr pfc_uint CONNECTION_HERTBEAT_RESPONSE = 0x10000004; //!<  value returned by type() from a pfc_heartbeat_request_response

struct SUSTAIN_FRAMEWORK_API pfc_heartbeat_request : pfc_message {
  pfc_uint _message_type = CONNECTION_HERTBEAT_REQUEST; //!< value returned by TYPE. Shoudld be constant for any specific message type unless a deseralization error has occured.
  pfc_ushort _port; //!< Stores a listinging port of the registered remote service
  pfc_protocol _protacol = pfc_protocol::pub_sub; //!< Stores teh protacol used byt the registered service
  pfc_string _name; //!< Human Readable Name of the Service
  pfc_string _address; //!< Valid URI for communicating with the service. Depending on the protcol implementation may be an IP4/IP6 or System Socket
  pfc_string _brief; //!< Human Description of the service and its feature set.

  ~pfc_heartbeat_request() override;

  size_t Length() const override; //!< Returns the length of the encoded message
  pfc_uint Type() const override; //!< Returns teh _message_type of an encoded message
  Error serialize(std::ostream& os) const override; //!< Sends teh seralized format of a pfc_heartbeat_request over the provided  ostream.
  Error deserialize(std::istream& is) override; //!< Marshalls a seralized service ServiceList and inflates it the data binding.
};
//!< Stream Operator for converting a pfc_heartbeat_request over an ostream
SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_heartbeat_request&);
//-----------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------
//! Equivalance Operator for pfc_heartbeat_requests
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_heartbeat_request&, const pfc_heartbeat_request&);
//! Implemented as ! operator==()
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_heartbeat_request&, const pfc_heartbeat_request&);

struct SUSTAIN_FRAMEWORK_API pfc_heartbeat_response : pfc_message {
  pfc_uint _message_type = CONNECTION_HERTBEAT_RESPONSE; //!< value returned by TYPE. Shoudld be constant for any specific message type unless a deseralization error has occured.
  pfc_ushort _port; //!< Stores a listinging port of the registered remote service
  pfc_protocol _protacol = pfc_protocol::pub_sub; //!< Stores teh protacol used byt the registered service
  pfc_string _name; //!< Human Readable Name of the Service
  pfc_string _address; //!< Valid URI for communicating with the service. Depending on the protcol implementation may be an IP4/IP6 or System Socket
  pfc_string _brief; //!< Human Description of the service and its feature set.

  ~pfc_heartbeat_response() override;

  size_t Length() const override; //!< Returns the length of the encoded message
  pfc_uint Type() const override; //!< Returns teh _message_type of an encoded message
  Error serialize(std::ostream& os) const override; //!< Sends teh seralized format of a pfc_heartbeat_response over the provided  ostream.
  Error deserialize(std::istream& is) override; //!< Marshalls a seralized service ServiceList and inflates it the data binding.
};
//!< Stream Operator for converting a pfc_heartbeat_response over an ostream
SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const pfc_heartbeat_response&);
//-----------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------
//! Equivalance Operator for pfc_heartbeat_responses
bool SUSTAIN_FRAMEWORK_API operator==(const pfc_heartbeat_response&, const pfc_heartbeat_response&);
//! Implemented as ! operator==()
bool SUSTAIN_FRAMEWORK_API operator!=(const pfc_heartbeat_response&, const pfc_heartbeat_response&);

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

/**
  *!  sizeof for pfc_types
  *!  Template specialization of of size_of_pfc-type for std::string
  *!  return strlen +/- null terminal
  **/
template <>
inline size_t size_of_pfc_type(const std::string& result)
{
  return result.length() * sizeof(std::string::value_type);
}

/**
  *!  sizeof for pfc_types
  *!  Template specialization for array types
  *!  return exsact bytesize of N * sizeof(T)
  **/
template <typename T, size_t N>
size_t size_of_pfc_type(const T (&)[N])
{
  constexpr size_t size = sizeof(T) * N;
  return size - 1;
}

/**
  *!  sizeof for pfc_types
  *!  Function overload to allow vaaradic templates to call size_of_pfc_type with no arguments.
  *!  return 0
  **/
constexpr size_t size_of_pfc_type()
{
  return 0;
}

/**
  *!  sizeof for pfc_types
  *!  Varadic template  used to determine the size of a pfc type.
  *!  used by passing decltype(member) in side template parms and member as an argument
  *!  Maybe nice enough to allow use with out <> on some compilers.
  **/
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

/**
  *!  Templete specialization of serialize_pfc_type for pfc_string
  **/
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

/**
  *!  Templete specialization of serialize_pfc_type for array types
  **/
template <typename T, size_t N>
Error serialize_pfc_type(std::ostream& is, T (&val)[N])
{
  constexpr size_t size = size_of_pfc_type(T) * N;
  is.write(reinterpret_cast<const char*>(&val[0]), size - 1);
  return Success();
}


/**
  *!  Veradic templte implementation of serialize_pfc_type
  *! Used to automate the seralization of standard types used simalerly to size_of_pfc_type
  **/
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


//!
//!  Tempalte Specialization of deserailzie_pfc_type for pfc_strings
//!
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

//!
//!  Tempalte Specialization of deserailzie_pfc_type for array types
//!
template <typename T, int N>
Error deserialize_pfc_type(std::istream& os, T (&result)[N])
{
  constexpr size_t size = size_of_pfc_type(T) * N;

  os.write(&result[0], size);
  os.write('\0', size_of_pfc_type('\0'));
  return Success();
}


//!
//!  Function overload of deserialize_pfc_type to allow veradic templates to call with no arguments. Returns Error::PFC_NONE
//!
inline Error deserialize_pfc_type(std::istream& os)
{
  return Success();
}

//!
//!  Veradic template for inflating seralization messages
//!
template <typename T, typename... VAR>
inline Error deserialize_pfc_type(std::istream& os,
                           T& first, VAR&... args)
{
  auto error = deserialize_pfc_type(os, first);
  error |= deserialize_pfc_type(os, args...);
  return error;
}
#pragma warning(pop)
}

#endif //SUSTAIN_PFCNW_PROTOCOL_H
