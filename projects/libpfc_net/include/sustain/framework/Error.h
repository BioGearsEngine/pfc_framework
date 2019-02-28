#ifndef SUSTAIN_PFCNW_ERROR_H
#define SUSTAIN_PFCNW_ERROR_H

#include <cstdint>
#include <ostream>

#include <sustain/framework/Exports.h>
namespace pfc {

class SUSTAIN_FRAMEWORK_API Error {
public:
  enum Code : uint64_t {
    //! Error Codes
    PFC_NONE = 1 << 0,
    PFC_IP_PARSE_ERROR = 1 << 1,
    PFC_BIND_ERROR = 1 << 2,
    PFC_ADDRESS_FAMILY_NOT_SUPPORTED = 1 << 3,
    PFC_ADDRESS_IN_USE = 1 << 4,
    PFC_ADDRESS_NOT_AVAILABLE = 1 << 5,
    PFC_IP_SERIALIZATION_ERROR = 1 << 6
  };

  Error();
  Error(uint64_t);
  Error(Code);
  Error(const Error&) = default;
  Error(Error&&) = default;

  bool operator==(const Error&) const;
  bool operator!=(const Error&) const;
  bool operator==(const uint64_t&) const;
  bool operator!=(const uint64_t&) const;

  Error& operator|=(const Code&);
  Error& operator|=(const Error&);
  Error& operator=(const Error&) = default;
  Error& operator=(Error&&) = default;
  Error& operator=(const Code&);

  inline bool is_ok() const { return _value == Code::PFC_NONE; }
  inline bool is_not_ok() const { return _value != Code::PFC_NONE; }

  void clear();

  operator uint64_t() const
  {
    return _value;
  }

  friend SUSTAIN_FRAMEWORK_API std::ostream& operator<<(std::ostream& ostr, const Error& e);

private:
  uint64_t _value;
};
SUSTAIN_FRAMEWORK_API std::ostream&  operator<<(std::ostream&, const Error&);
};

#endif //SUSTAIN_PFCNW_ERROR_H