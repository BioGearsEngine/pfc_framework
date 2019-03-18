#ifndef SUSTAIN_FRAMEWORK_NET_URI_H
#define SUSTAIN_FRAMEWORK_NET_URI_H

#include <regex>
#include <string>

#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Error.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API URI {
public:
  URI(std::string uri);
  URI(std::string transport, std::string address, uint16_t port = 0);

  URI(const URI&) = default;
  URI(URI&&) = default;
  URI& operator=(const URI&) = default;
  URI& operator=(URI&&) = default;

  bool operator==(const URI&) const;
  bool operator!=(const URI&) const;

  std::string transport() const { return _transport; }
  std::string address() const { return _address; }
  std::string port() const { return _port; }

  const char* c_str() const;

  Error error() const;
  bool is_valid() const;

private:
#pragma warning(push,0)
  std::string _transport;
  std::string _address;
  std::string _port;
  std::string _endpoint;
#pragma warning(pop)
  Error _ec;
};

std::ostream& operator<<(std::ostream&, const URI&);

//-----------------------------------------------------------------------------
// Implementation
//-----------------------------------------------------------------------------
inline URI::URI(std::string uri)
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
inline URI::URI(std::string transport, std::string address, uint16_t port)
{
  _transport = transport;
  _address = address;
  _port = std::to_string(port);

  if (port) {
    _address = _transport + "://" + _address + ":" + _port;
  } else {
    _address = _transport + "://" + _address;
  }

  std::regex rx(R"REGEX((\w+)://([A-Za-z0-9_.]+)(:(\d+)){0,1})REGEX");
  std::smatch matches;

  if (!std::regex_match(_address, matches, rx)) {
    _ec = Error::PFC_IP_PARSE_ERROR;
  }
}
//-----------------------------------------------------------------------------
inline bool URI::operator==(const URI& rhs) const
{
  return _transport == rhs._transport
    && _address == rhs._address
    && _port == rhs._port;
}
//-----------------------------------------------------------------------------
inline bool URI::operator!=(const URI& rhs) const
{
  return !(*this == rhs);
}
//-----------------------------------------------------------------------------
inline const char* URI::c_str() const
{
  return _address.c_str();
}
//-----------------------------------------------------------------------------
inline Error URI::error() const
{
  return _ec;
}
//-----------------------------------------------------------------------------
inline bool URI::is_valid() const
{
  return _ec.is_ok();
}
//-----------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& os, const URI& uri)
{
  os << uri.c_str();
  return os;
}
}

#endif //SUSTAIN_FRAMEWORK_NET_URI_H