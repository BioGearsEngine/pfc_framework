#include <sustain/framework/Error.h>

namespace pfc {
Error::Error()
  : _value(PFC_NONE)
{
}
//-----------------------------------------------------------------------------
Error::Error(uint64_t value)
  : _value(value)
{
}
//-----------------------------------------------------------------------------
Error::Error(Code value)
  : _value(value)
{
}
//-----------------------------------------------------------------------------
bool Error::operator==(const Error& rhs) const
{
  return _value == rhs._value;
}
//-----------------------------------------------------------------------------
bool Error::operator!=(const Error& rhs) const
{
  return !(_value == rhs._value);
}
//-----------------------------------------------------------------------------
bool Error::operator==(const uint64_t& rhs) const
{
  return _value & rhs;
}
//-----------------------------------------------------------------------------
bool Error::operator!=(const uint64_t& rhs) const
{
  return !(_value == rhs);
}
//-----------------------------------------------------------------------------
Error& Error::operator=(const Code& rhs)
{
  _value = rhs;
  return *this;
}
//-----------------------------------------------------------------------------
Error& Error::operator|=(const Code& rhs)
{
  _value |= rhs;
  return *this;
}
//-----------------------------------------------------------------------------
Error& Error::operator|=(const Error& rhs)
{
  _value |= rhs._value;
  return *this;
}
//-----------------------------------------------------------------------------4
void Error::clear()
{
  _value = Error::PFC_NONE;
}
//-----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& ostr, const Error& e)
{
  switch (static_cast<uint64_t>(e)) {
  case Error::PFC_NONE:
    ostr << "Error::"
         << "None";
    break;
  case Error::PFC_IP_PARSE_ERROR:
    ostr << "Error::"
         << "Ip_Parse_Error";
    break;
  case Error::PFC_BIND_ERROR:
    ostr << "Error::"
         << "Bind_Error";
    break;
  case Error::PFC_ADDRESS_FAMILY_NOT_SUPPORTED:
    ostr << "Error::"
         << "Address_Family_Not_Supported";
    break;
  case Error::PFC_ADDRESS_IN_USE:
    ostr << "Error::"
         << "Address_In_Use";
    break;
  case Error::PFC_ADDRESS_NOT_AVAILABLE:
    ostr << "Error::"
         << "Address_Not_Avaliable";
    break;
  default:
    break;
  }
  return ostr;
}
//-----------------------------------------------------------------------------
}