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

#include <sustain/framework/util/Error.h>

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
bool Error::operator==(const Code& rhs) const
{
  return _value & rhs;
}
//-----------------------------------------------------------------------------
bool Error::operator!=(const Code& rhs) const
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
//-----------------------------------------------------------------------------
Error Error::operator|(const Error& rhs) const { return {_value | rhs._value}; }
//----------------------------------------------------------------------------
Error Error::operator&(const Error& rhs) const { return {_value & rhs._value}; }
//----------------------------------------------------------------------------
Error Error::operator|(const Code& rhs) const { return  {_value | rhs}; }
//----------------------------------------------------------------------------
Error Error::operator&(const Code& rhs) const { return  {_value & rhs}; }
//-----------------------------------------------------------------------------
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
