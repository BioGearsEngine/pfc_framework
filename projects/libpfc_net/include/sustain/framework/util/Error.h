#ifndef SUSTAIN_PFCNW_ERROR_H
#define SUSTAIN_PFCNW_ERROR_H

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

#include <cstdint>
#include <ostream>

#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Constants.h>
namespace pfc {

//!
//! Global PFC Error class
//! Operats as a Bit Array of possible errors
//! Users can compare Errors using | and & operators
class SUSTAIN_FRAMEWORK_API Error {
public:

  //!
  //! Valid PFC Error codes 
  //! \brief all error codes are a power of two for a maximum of 63 Error codes
  //!        Error codes can be combined together when multiple errors occur and checked
  //!        using the &= and && operators to see if a specific error occured.
  enum Code : uint64_t {
    //! Error Codes
    PFC_NONE = 0 << 0,
    PFC_IP_PARSE_ERROR = 1 << 0,
    PFC_BIND_ERROR = 1 << 1,
    PFC_ADDRESS_FAMILY_NOT_SUPPORTED = 1 << 2,
    PFC_ADDRESS_IN_USE = 1 << 3,
    PFC_PROTOCOL_NOT_SUPPORTED = 1 << 4,
    PFC_IP_SERIALIZATION_ERROR = 1 << 5,
    PFC_SOCKET_LIMIT_REACHED = 1 << 6,
    PFC_LIBRARY_SHUTDOWN = 1 << 7,
    PFC_INVALID_SOCKET = 1 << 8,
    PFC_INVALID_ENDPOINT = 1 << 9,
    PFC_BAD_OPERATION = 1 << 10,
    PFC_INTERUPT = 1 << 11,
    PFC_TIMEOUT = 1 << 12,
    PFC_ADDRESS_NOT_AVAILABLE = 1 << 13
  };

  Error();
  Error(uint64_t);
  Error(Code);
  Error(const Error&) = default;                    //!< Default Copy Constructor
  Error(Error&&) = default;                         //!< Default Move Constructor

  Error& operator=(const Error&) = default;         //!< Default Assignment Operator
  Error& operator=(Error&&) = default;              //!< Default Move Operator
  Error& operator=(const Code&);

  bool operator==(const Error&) const;
  bool operator!=(const Error&) const;
  bool operator==(const Code&) const;
  bool operator!=(const Code&) const;

  Error& operator|=(const Code&);
  Error& operator|=(const Error&);
  Error operator|(const Error&) const;
  Error operator&(const Error&)const;
  Error operator|(const Code&) const;
  Error operator&(const Code&)const;

  operator bool() const { return _value != Code::PFC_NONE; }          //!<  Boolean Operator returns true when is_not_ok() is true
  inline bool is_ok() const { return _value == Code::PFC_NONE; }      //!< Shortcut comparison for is Error a Success()
  inline bool is_not_ok() const { return _value != Code::PFC_NONE; }  //!< Shortcut comparison for is Error not equal to Success()

  void clear();

  friend SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream& ostr, const Error& e);

private:
  uint64_t _value;
};

//!
//!  Helper function for generating a non-error result
//!  This simply reads better in code
//!  \brief return Success(); vs return Error(Error::code::PFC_NONE);
//!
inline Error Success() { return { Error::Code::PFC_NONE }; }

SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream&, const Error&);
SUSTAIN_FRAMEWORK_API Error operator|(const Error::Code&, const Error::Code&);
};

#endif //SUSTAIN_PFCNW_ERROR_H
