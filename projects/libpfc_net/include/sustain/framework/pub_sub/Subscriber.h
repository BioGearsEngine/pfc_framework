#ifndef SUSTAIN_SERVICE_H
#define SUSTAIN_SERVICE_H

//!
//! \author Steven A White
//! \date   2019-02-14
//!
//! \brief Defines a Subscriber Subscriber Class
//!        Class meets the speficiation of the Sustain Framework
//!

#include <memory>
#include <string>
#include <cstdint>
#include <functional>

#include <sustain/framework/util/Error.h>
#include <sustain/framework/util/Constants.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API Subscriber {
public:
  Subscriber(std::string service_name, std::string multicast_address);
  Subscriber(const Subscriber&) = delete;
  Subscriber(Subscriber&&);
  ~Subscriber();

  std::string address();
  uint16_t port();

  void send( std::ostream& );
  void async_send( std::ostream& );

  bool Valid();
  Error Error();

  Subscriber& operator=(const Subscriber&) = delete;
  Subscriber& operator=(Subscriber&&);

private:
  struct Implementation;
  #pragma warning(suppress:4251)
  std::unique_ptr<Implementation> _impl;

};
} //namespace pfc

#endif //SUSTAIN_SERVICE_H