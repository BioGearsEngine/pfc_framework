#ifndef SUSTAIN_PFCNW_ERROR_H
#define SUSTAIN_PFCNW_ERROR_H

#include <cstdint>
#include <ostream>

#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Constants.h>
namespace pfc {

class SUSTAIN_FRAMEWORK_API Error {
public:
  enum Code : uint64_t {
    //! Error Codes
    PFC_NONE = 0 << 0,
    PFC_IP_PARSE_ERROR = 1 << 0,
    PFC_BIND_ERROR = 1 << 1,
    PFC_ADDRESS_FAMILY_NOT_SUPPORTED = 1 << 2,
    PFC_ADDRESS_IN_USE = 1 << 3,
    PFC_ADDRESS_NOT_AVAILABLE = 1 << 4,
    PFC_IP_SERIALIZATION_ERROR = 1 << 5
  };

  Error();
  Error(uint64_t);
  Error(Code);
  Error(const Error&) = default;
  Error(Error&&) = default;

  Error& operator=(const Error&) = default;
  Error& operator=(Error&&) = default;
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

  operator bool() const { return _value != Code::PFC_NONE; }
  inline bool is_ok() const { return _value == Code::PFC_NONE; }
  inline bool is_not_ok() const { return _value != Code::PFC_NONE; }

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
SUSTAIN_FRAMEWORK_API Error& operator|(const Error::Code&, const Error::Code&);
};

#endif //SUSTAIN_PFCNW_ERROR_H