#ifndef SUSTAIN_FRAMEWORK_NET_URI_H
#define SUSTAIN_FRAMEWORK_NET_URI_H

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

#include <regex>
#include <string>

#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Error.h>

namespace pfc {

//!
//!  Class for encoding Universal Resource Indentifiers 
//! <a href="https://en.wikipedia.org/wiki/Uniform_Resource_Identifier">Wikipedia Entry</a>
//! <a href="https://tools.ietf.org/html/rfc3986">Full Specification</a>
class SUSTAIN_FRAMEWORK_API URI {
public:
  URI(std::string uri);
  URI(std::string transport, std::string address, uint16_t port = 0);

  URI(const URI&) = default;                       //!< Default Copy Constructor
  URI(URI&&) = default;                            //!< Default Move Constructor
  URI& operator=(const URI&) = default;            //!< Default Copy operator
  URI& operator=(URI&&) = default;                 //!< Default Move Operator

  bool operator==(const URI&) const;
  bool operator!=(const URI&) const;

  std::string transport() const { return _transport; }       //!< Returns the current transport as a string 
  std::string address() const { return _address; }           //!< Returns the current address as a string
  std::string port() const { return _port; }                 //!< Returns the current port as a string
  int port_i() const { return std::stoi(_port); } //!< Returns the current port as a string
  const char* c_str() const; //!< Returns the current port as an int

  Error error() const;
  bool is_valid() const;

private:
#pragma warning(push,0)
  std::string _transport;      //!<  Protocol scheme http/ftp/tcp/udp...
  std::string _address;        //!<  Host address 
  std::string _port;           //!<  Port on Host Address to be used for binding (defaults to 80, but future implementations may pick defaults based on transport)
  std::string _endpoint;       //!<  Full copy of original end point given
#pragma warning(pop)
  Error _ec;
};

std::ostream& operator<<(std::ostream&, const URI&);

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------
//! \param uri [IN] - Valid URI format to be stored inside the the struct
inline URI::URI(std::string uri)
  : _port("80")
{
  std::regex rx(R"REGEX((\w+)://([A-Za-z0-9_.]+)(:(\d+)){0,1})REGEX");
  std::smatch matches;

  if (std::regex_match(uri, matches, rx)) {
    _transport = matches[0];
    _address = matches[1];
    if (matches.size() == 4) {
      _port = matches[0];
    }
    _endpoint = std::move(uri);
  } else {
    _ec = Error::PFC_IP_PARSE_ERROR;
  }
}
//-----------------------------------------------------------------------------
//!   \param transport [IN]  -- Transport protocol of the URI 
//!   \param address  [IN]   -- Host address as either a DNS/IPv4/IPv6 valid address
//!   \param port [IN]       -- Valid 2 byte port number 
inline URI::URI(std::string transport, std::string address, uint16_t port)
{
  _transport = std::move(transport);
  _address = std::move(address);
  _port = std::to_string(port);

  //Rencenty changed _address to _endpoint for lines 94,96,102 -- Perform regression test
  if (port) {
    _endpoint = _transport + "://" + _address + ":" + _port;
  } else {
    _endpoint = _transport + "://" + _address;
  }

  std::regex rx(R"REGEX((\w+)://([A-Za-z0-9_.]+)(:(\d+)){0,1})REGEX");
  std::smatch matches;

  if (!std::regex_match(_endpoint, matches, rx)) {
    _ec = Error::PFC_IP_PARSE_ERROR;
  }
}
//-----------------------------------------------------------------------------
//! Comparison Operator true only when all three componets of a URI are equal
//! \param rhs [IN] object to be copied
inline bool URI::operator==(const URI& rhs) const
{
  return _transport == rhs._transport
    && _address == rhs._address
    && _port == rhs._port;
}
//-----------------------------------------------------------------------------
//! same as ! oeprator==()
// \param rhs [IN] Object to be compared to self
inline bool URI::operator!=(const URI& rhs) const
{
  return !(*this == rhs);
}
//-----------------------------------------------------------------------------
//! \return char const* - Valid until next call to address(std::string)
inline const char* URI::c_str() const
{
  return _address.c_str();
}
//-----------------------------------------------------------------------------
//! \return Error - Success() or the last Error State of the URI
inline Error URI::error() const
{
  return _ec;
}
//-----------------------------------------------------------------------------
//! \return short cut of calling Error().is_ok()
inline bool URI::is_valid() const
{
  return _ec.is_ok();
}
//-----------------------------------------------------------------------------
//!  Stream Operator for URIs 
inline std::ostream& operator<<(std::ostream& os, const URI& uri)
{
  os << uri.c_str();
  return os;
}
}

#endif //SUSTAIN_FRAMEWORK_NET_URI_H
