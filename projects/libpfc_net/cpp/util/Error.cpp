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

#include <sustain/framework/util/Error.h>

namespace pfc {
//!
//! Default constructor with value of Success()
Error::Error()
  : _value(PFC_NONE)
{
}
//-----------------------------------------------------------------------------
//!  Error code for constructing arbitary Error combinations
//! \param value [IN] some int which represents multiple error codes or an error code mask
Error::Error(uint64_t value)
  : _value(value)
{
}
//-----------------------------------------------------------------------------
//! Code based constructor
//! \param value [IN] single error code the new Error will preresent
Error::Error(Code value)
  : _value(value)
{
}
//-----------------------------------------------------------------------------
//! \param rhs [IN] -- rhs operation
//! \return  True iff _value == rhs._value
bool Error::operator==(const Error& rhs) const
{
  return _value == rhs._value;
}
//-----------------------------------------------------------------------------
//! \param rhs [IN] -- rhs operation
//! \return true  when any bit in _value differs from rhs._value
bool Error::operator!=(const Error& rhs) const
{
  return !(_value == rhs._value);
}
//-----------------------------------------------------------------------------
//! \param rhs [IN] -- rhs operation
//! \return bitwise AND of rhs and _value; True iff _value == rhs._value
bool Error::operator==(const Code& rhs) const
{
  return _value & rhs;
}
//-----------------------------------------------------------------------------
//! \param rhs [IN] -- rhs operation
//! \return Bool  true when _value != to rhs
bool Error::operator!=(const Code& rhs) const
{
  return !(_value == rhs);
}
//-----------------------------------------------------------------------------
//! Assigns a code to Self 
//! \param rhs [IN] -- rhs operation
//! \return Modifed version of self as a result of the operation
Error& Error::operator=(const Code& rhs)
{
  _value = rhs;
  return *this;
}
//-----------------------------------------------------------------------------
//! Bit wise OR of Code and Self to represent multiple errors in a single 64bit int
//! \param rhs [IN] -- rhs operation
//! \return Modifed version of self as a result of the operation
Error& Error::operator|=(const Code& rhs)
{
  _value |= rhs;
  return *this;
}
//-----------------------------------------------------------------------------
////! Bit wise OR of Error and Self to represent multiple errors in a single 64bit int
//! \param rhs [IN] -- rhs operation
//! \return Modifed version of self as a result of the operation
Error& Error::operator|=(const Error& rhs)
{
  _value |= rhs._value;
  return *this;
}
//-----------------------------------------------------------------------------
//! BIT WISE OR of an error code allows stacking of error codes
//! \param rhs [IN] -- rhs operation
//! \return Error newly constructed error code that is the result of the operation
Error Error::operator|(const Error& rhs) const { return {_value | rhs._value}; }
//----------------------------------------------------------------------------
//! BIT wise and of Error code allows comparison against a MASK
//! \param rhs [IN] -- rhs operation
//! \return Error newly constructed error code that is the result of the operation
Error Error::operator&(const Error& rhs) const { return {_value & rhs._value}; }
//----------------------------------------------------------------------------
//! BITISE OR for an ERROR Code can be used to stack error codes
//! \param rhs [IN] -- rhs operation
//! \return Error newly constructed error code that is the result of the operation
Error Error::operator|(const Code& rhs) const { return  {_value | rhs}; }
//----------------------------------------------------------------------------
//! BITWISE AND of IMPLEMENTATION ERROR CODE CAN BE USED TO check ERROR against a MASK
//! \param rhs [IN] -- rhs operation
//! \return Error newly constructed error code that is the result of the operation
Error Error::operator&(const Code& rhs) const { return  {_value & rhs}; }
//-----------------------------------------------------------------------------
//! Clear existing error to Success()
void Error::clear()
{
  _value = Error::PFC_NONE;
}
//-----------------------------------------------------------------------------
//!
//! \param ostr [OUT] -- Stream to which the message will be appended 
//! \param e [IN] -- Error to be converted to a string
//! \return std::ostream& - modified ostr to allow call chaining
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
//! Convience function for merging Error::Codes inline
//!  \param lhs -- Left Hand Side Code
//! \param rhs -- Right hand side Code
//! \return Error -- pfc::Error which has both the lhs and rhs bits high
Error operator|(const Error::Code& lhs, const Error::Code& rhs)
{
  return Error(lhs) | rhs;
}
}
