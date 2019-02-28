#ifndef SUSTAIN_SERVICE_H
#define SUSTAIN_SERVICE_H

//!
//! \author Steven A White
//! \date   2019-02-14
//!
//! \brief Defines a Service Service Class
//!        Class meets the speficiation of the Sustain Framework
//!

#include <memory>
#include <string>
#include <cstdint>

#include <sustain/framework/Error.h>
#include <sustain/framework/Constants.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API Service {
public:
  Service(std::string serice_name, std::string multicast_address);
  Service(const Service&) = delete;
  Service(Service&&);
  ~Service();

  std::string name();
  std::string address();
  uint16_t port();

  void start();
  void stop();
  void join();

  bool Valid();
  Error Error();

  Service& operator=(const Service&) = delete;
  Service& operator=(Service&&);

private:
  struct Implementation;
  #pragma warning(suppress:4251)
  std::unique_ptr<Implementation> _impl;

};
} //namespace pfc

#endif //SUSTAIN_SERVICE_H