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


#include "sustain/framework/Protocol.h"

#include <sstream>
#include <string>

namespace pfc {

//! Ostream operator for pfc_protocol
std::ostream& operator<<(std::ostream& os, const pfc_protocol& rhs)
{
  os << ((rhs == pub_sub) ? "pub_sub" : "req_rep");
  return os;
}
//-----------------------------------------------------------------------------
//! Service Announcement Broadcast
//! \brief: Used to declare yourself to a registry and echoed to all other services
//
pfc_service_announcement::~pfc_service_announcement()
{
  _name.resize(0);
  _address.resize(0);
  _brief.resize(0);
};
//-----------------------------------------------------------------------------
// \return size_t length of message once serialized
size_t pfc_service_announcement::Length() const
{
  size_t length = size_of_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(_message_type, _port, _protacol, _name, _address, _brief);
  return length;
}
//-----------------------------------------------------------------------------
// \return pfc_uint -- Message Type
pfc_uint pfc_service_announcement::Type() const
{
  return _message_type;
}
//-----------------------------------------------------------------------------
//! Serializes a given pfc_service_announcement to an ostream
// \param os [IN,OUT] -- Outbound stream to contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_service_announcement::serialize(std::ostream& os) const
{

  return serialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(os, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
//! Deserializes a given pfc_service_announcement from an istream
// \param is [IN,OUT] -- Input stream that contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_service_announcement::deserialize(std::istream& is)
{
  return deserialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(is, _message_type, _port, _protacol, _name, _address, _brief);
}

//! ostream oeprator for pfc_service_announcement messages
std::ostream& operator<<(std::ostream& os, const pfc_service_announcement& msg)
{
  os << "pfc_service_announcement("
     << "name=" << msg._name << ","
     << " address=" << msg._protacol << "://" << msg._address << ":" << msg._port << ","
     << " brief=" << msg._brief
     << ")";
  return os;
}
//-----------------------------------------------------------------------------
//!
//!  comparison of mesages
//!
//-----------------------------------------------------------------------                                                                                                                          
bool operator==(const pfc_service_announcement& lhs, const pfc_service_announcement& rhs)
{

  return lhs._message_type == rhs._message_type
    && lhs._protacol == rhs._protacol
    && lhs._port == rhs._port
    && lhs._name == rhs._name
    && lhs._address == rhs._address
    && lhs._brief == rhs._brief;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_service_announcement& lhs, const pfc_service_announcement& rhs)
{
  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
//! Service Signoff 
//! \brief: A service should send this to a registry when properly shutdown. Will be sent to all registered services/client as well
//
pfc_service_signoff::~pfc_service_signoff()
{
  _name.resize(0);
  _address.resize(0);
  _brief.resize(0);
};
//-----------------------------------------------------------------------------
// \return size_t length of message once serialized
size_t pfc_service_signoff::Length() const
{
  size_t length = size_of_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(_message_type, _port, _protacol, _name, _address, _brief);
  return length;
}
//-----------------------------------------------------------------------------
// \return pfc_uint -- Message Type
pfc_uint pfc_service_signoff::Type() const
{
  return _message_type;
}
//-----------------------------------------------------------------------------
//! Serializes a given pfc_service_signoff to an ostream
// \param os [IN,OUT] -- Outbound stream to contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_service_signoff::serialize(std::ostream& os) const
{

  return serialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(os, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
//! Deserializes a given pfc_service_signoff from an istream
// \param is [IN,OUT] -- Input stream that contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_service_signoff::deserialize(std::istream& is)
{
  return deserialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(is, _message_type, _port, _protacol, _name, _address, _brief);
}

//! ostream oeprator for pfc_service_signoff messages
std::ostream& operator<<(std::ostream& os, const pfc_service_signoff& msg)
{
  os << "pfc_service_signoff("
     << "name=" << msg._name << ","
     << " address=" << msg._protacol << "://" << msg._address << ":" << msg._port << ","
     << " brief=" << msg._brief
     << ")";
  return os;
}
//-----------------------------------------------------------------------------
//!
//!  comparison of mesages
//!
//-----------------------------------------------------------------------
bool operator==(const pfc_service_signoff& lhs, const pfc_service_signoff& rhs)
{

  return lhs._message_type == rhs._message_type
    && lhs._protacol == rhs._protacol
    && lhs._port == rhs._port
    && lhs._name == rhs._name
    && lhs._address == rhs._address
    && lhs._brief == rhs._brief;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_service_signoff& lhs, const pfc_service_signoff& rhs)
{
  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
//! PFC Registry Request/Response
//! \brief: New Services can use a Registry Request to get a list of all existing services
//!         While a ServiceRegistryResponse includes a list of all ServiceAnnouncments
//          Pure clients will also need Service List as they do not ServiceBroadcast
pfc_registry_request::~pfc_registry_request()
{
  _name.resize(0);
  _address.resize(0);
  _brief.resize(0);
};
//-----------------------------------------------------------------------------
// \return size_t length of message once serialized
size_t pfc_registry_request::Length() const
{
  size_t length = size_of_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(_message_type, _port, _protacol, _name, _address, _brief);
  return length;
}
//-----------------------------------------------------------------------------
// \return pfc_uint -- Message Type
pfc_uint pfc_registry_request::Type() const
{
  return _message_type;
}
//-----------------------------------------------------------------------------
//! Serializes a given pfc_registry_request to an ostream
// \param os [IN,OUT] -- Outbound stream to contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_registry_request::serialize(std::ostream& os) const
{

  return serialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(os, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
//! Deserializes a given pfc_registry_request from an istream
// \param is [IN,OUT] -- Input stream that contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_registry_request::deserialize(std::istream& is)
{
  return deserialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(is, _message_type, _port, _protacol, _name, _address, _brief);
}

//! ostream oeprator for pfc_registry_request messages
std::ostream& operator<<(std::ostream& os, const pfc_registry_request& msg)
{
  os << "pfc_registry_request("
     << "name=" << msg._name << ","
     << " address=" << msg._protacol << "://" << msg._address << ":" << msg._port << ","
     << " brief=" << msg._brief
     << ")";
  return os;
}
//-----------------------------------------------------------------------------
//!
//!  comparison of mesages
//!
//-----------------------------------------------------------------------
bool operator==(const pfc_registry_request& lhs, const pfc_registry_request& rhs)
{

  return lhs._message_type == rhs._message_type
    && lhs._protacol == rhs._protacol
    && lhs._port == rhs._port
    && lhs._name == rhs._name
    && lhs._address == rhs._address
    && lhs._brief == rhs._brief;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_registry_request& lhs, const pfc_registry_request& rhs)
{
  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
//! Service Announcement Broadcast
//! \brief: Used to declare yourself to a registry and echoed to all other services
//
pfc_registry_response::~pfc_registry_response()
{
  _name.resize(0);
  _address.resize(0);
  _brief.resize(0);
};
//-----------------------------------------------------------------------------
// \return size_t length of message once serialized
size_t pfc_registry_response::Length() const
{
  size_t length = size_of_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(_message_type, _port, _protacol, _name, _address, _brief);
  return length;
}
//-----------------------------------------------------------------------------
// \return pfc_uint -- Message Type
pfc_uint pfc_registry_response::Type() const
{
  return _message_type;
}
//-----------------------------------------------------------------------------
//! Serializes a given pfc_registry_response to an ostream
// \param os [IN,OUT] -- Outbound stream to contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_registry_response::serialize(std::ostream& os) const
{

  return serialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(os, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
//! Deserializes a given pfc_registry_response from an istream
// \param is [IN,OUT] -- Input stream that contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_registry_response::deserialize(std::istream& is)
{
  return deserialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(is, _message_type, _port, _protacol, _name, _address, _brief);
}

//! ostream oeprator for pfc_registry_response messages
std::ostream& operator<<(std::ostream& os, const pfc_registry_response& msg)
{
  os << "pfc_registry_response("
     << "name=" << msg._name << ","
     << " address=" << msg._protacol << "://" << msg._address << ":" << msg._port << ","
     << " brief=" << msg._brief
     << ")";
  return os;
}
//-----------------------------------------------------------------------------
//!
//!  comparison of mesages
//!
//-----------------------------------------------------------------------
bool operator==(const pfc_registry_response& lhs, const pfc_registry_response& rhs)
{

  return lhs._message_type == rhs._message_type
    && lhs._protacol == rhs._protacol
    && lhs._port == rhs._port
    && lhs._name == rhs._name
    && lhs._address == rhs._address
    && lhs._brief == rhs._brief;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_registry_response& lhs, const pfc_registry_response& rhs)
{
  return !(lhs == rhs);
}



//-----------------------------------------------------------------------------
//! PFC Registry Request/Response
//! \brief: New Services can use a Registry Request to get a list of all existing services
//!         While a ServiceRegistryResponse includes a list of all ServiceAnnouncments
//          Pure clients will also need Service List as they do not ServiceBroadcast
pfc_heartbeat_request::~pfc_heartbeat_request()
{
  _name.resize(0);
  _address.resize(0);
  _brief.resize(0);
};
//-----------------------------------------------------------------------------
// \return size_t length of message once serialized
size_t pfc_heartbeat_request::Length() const
{
  size_t length = size_of_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(_message_type, _port, _protacol, _name, _address, _brief);
  return length;
}
//-----------------------------------------------------------------------------
// \return pfc_uint -- Message Type
pfc_uint pfc_heartbeat_request::Type() const
{
  return _message_type;
}
//-----------------------------------------------------------------------------
//! Serializes a given pfc_heartbeat_request to an ostream
// \param os [IN,OUT] -- Outbound stream to contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_heartbeat_request::serialize(std::ostream& os) const
{

  return serialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(os, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
//! Deserializes a given pfc_heartbeat_request from an istream
// \param is [IN,OUT] -- Input stream that contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_heartbeat_request::deserialize(std::istream& is)
{
  return deserialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(is, _message_type, _port, _protacol, _name, _address, _brief);
}

//! ostream oeprator for pfc_heartbeat_request messages
std::ostream& operator<<(std::ostream& os, const pfc_heartbeat_request& msg)
{
  os << "pfc_heartbeat_request("
     << "name=" << msg._name << ","
     << " address=" << msg._protacol << "://" << msg._address << ":" << msg._port << ","
     << " brief=" << msg._brief
     << ")";
  return os;
}
//-----------------------------------------------------------------------------
//!
//!  comparison of mesages
//!
//-----------------------------------------------------------------------
bool operator==(const pfc_heartbeat_request& lhs, const pfc_heartbeat_request& rhs)
{

  return lhs._message_type == rhs._message_type
    && lhs._protacol == rhs._protacol
    && lhs._port == rhs._port
    && lhs._name == rhs._name
    && lhs._address == rhs._address
    && lhs._brief == rhs._brief;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_heartbeat_request& lhs, const pfc_heartbeat_request& rhs)
{
  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
//! Service Announcement Broadcast
//! \brief: Used to declare yourself to a heartbeat and echoed to all other services
//
pfc_heartbeat_response::~pfc_heartbeat_response()
{
  _name.resize(0);
  _address.resize(0);
  _brief.resize(0);
};
//-----------------------------------------------------------------------------
// \return size_t length of message once serialized
size_t pfc_heartbeat_response::Length() const
{
  size_t length = size_of_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(_message_type, _port, _protacol, _name, _address, _brief);
  return length;
}
//-----------------------------------------------------------------------------
// \return pfc_uint -- Message Type
pfc_uint pfc_heartbeat_response::Type() const
{
  return _message_type;
}
//-----------------------------------------------------------------------------
//! Serializes a given pfc_heartbeat_response to an ostream
// \param os [IN,OUT] -- Outbound stream to contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_heartbeat_response::serialize(std::ostream& os) const
{

  return serialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(os, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
//! Deserializes a given pfc_heartbeat_response from an istream
// \param is [IN,OUT] -- Input stream that contains a message
// \return Error -- Success() unless an Error occured during deserialization
Error pfc_heartbeat_response::deserialize(std::istream& is)
{
  return deserialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(is, _message_type, _port, _protacol, _name, _address, _brief);
}

//! ostream oeprator for pfc_heartbeat_response messages
std::ostream& operator<<(std::ostream& os, const pfc_heartbeat_response& msg)
{
  os << "pfc_heartbeat_response("
     << "name=" << msg._name << ","
     << " address=" << msg._protacol << "://" << msg._address << ":" << msg._port << ","
     << " brief=" << msg._brief
     << ")";
  return os;
}
//-----------------------------------------------------------------------------
//!
//!  comparison of mesages
//!
//-----------------------------------------------------------------------
bool operator==(const pfc_heartbeat_response& lhs, const pfc_heartbeat_response& rhs)
{

  return lhs._message_type == rhs._message_type
    && lhs._protacol == rhs._protacol
    && lhs._port == rhs._port
    && lhs._name == rhs._name
    && lhs._address == rhs._address
    && lhs._brief == rhs._brief;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_heartbeat_response& lhs, const pfc_heartbeat_response& rhs)
{
  return !(lhs == rhs);
}
}
