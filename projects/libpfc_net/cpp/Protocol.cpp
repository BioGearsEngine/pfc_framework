#include "sustain/framework/Protocol.h"

#include <sstream>
#include <string>

namespace pfc {

std::ostream& operator<<(std::ostream& os, const pfc_protocol& rhs)
{
  os << ((rhs == pub_sub) ? "pub_sub" : "req_rep");
  return os;
}
//-----------------------------------------------------------------------------
pfc_service_announcment::~pfc_service_announcment()
{
  _name.resize(0);
  _address.resize(0);
  _brief.resize(0);
};
//-----------------------------------------------------------------------------
size_t pfc_service_announcment::Length() const
{
  size_t length = size_of_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(_message_type, _port, _protacol, _name, _address, _brief);
  return length;
}
//-----------------------------------------------------------------------------
pfc_uint pfc_service_announcment::Type() const
{
  return _message_type;
}
//-----------------------------------------------------------------------------
Error pfc_service_announcment::serialize(std::ostream& os) const
{

  return serialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(os, _message_type, _port, _protacol, _name, _address, _brief);
}
//-----------------------------------------------------------------------------
Error pfc_service_announcment::deserialize(std::istream& is)
{
  return deserialize_pfc_type<
    decltype(_message_type), decltype(_port), decltype(_protacol), decltype(_name), decltype(_address), decltype(_brief)>(is, _message_type, _port, _protacol, _name, _address, _brief);
}
std::ostream& operator<<(std::ostream& os, const pfc_service_announcment& msg)
{
  os << "pfc_service_announcment("
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
bool operator==(const pfc_service_announcment& lhs, const pfc_service_announcment& rhs)
{

  return lhs._message_type == rhs._message_type
    && lhs._protacol == rhs._protacol
    && lhs._port == rhs._port
    && lhs._name == rhs._name
    && lhs._address == rhs._address
    && lhs._brief == rhs._brief;
}
//-----------------------------------------------------------------------
bool operator!=(const pfc_service_announcment& lhs, const pfc_service_announcment& rhs)
{
  return !(lhs == rhs);
}
}
