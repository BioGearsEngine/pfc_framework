#ifndef SUSTAIN_SERVICE_H
#define SUSTAIN_SERVICE_H

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


//!
//! \author Steven A White
//! \date   2019-02-14
//! \file
//! \brief Defines a Service Service Class
//!        Class meets the speficiation of the Sustain Framework
//!

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <sustain/framework/util/Constants.h>
#include <sustain/framework/util/Error.h>
#include <sustain/framework/net/Uri.h>

namespace pfc {
struct pfc_service_announcment; 

//!
//!  Base class for PFC Services.
//!  Services can be any information providing broadcaster. All services will automatically register
//!  with the PFC broker using UDP/Multicast on the braodcast channel provided at construction
//!
//!  Service Implementation can be pub_sub req_rep or survey and must be implemented by the derived class to provide
//!  Functionality beyond registration
//!

class SUSTAIN_FRAMEWORK_API Service {
public:

  //!
  //!  Configuration struct for the service. Defines teh connection information clients will use
  //!  to work with the service once they receive word from the broadcast channel of its existance. 
  //!  PFC Registry will store a service list provided a heartbeat continues to confirm the existance of a registrant.

  struct Config {
    //!
    //! Enum for specifying protocol
    enum protocol { pub_sub,
                    req_rep,
                    survey,
    } style;                     //!< Style the service will use for interacting with clients
    uint16_t port;               //!< Port the service runs on
    std::string name;            //!< Human friendly name of the service
    URI address;                 //!< Service URI other clients will use to connect to this service
    std::string brief;           //!< Details the expected purpose of this service and possibly a guide on how to interact with it or where documentation can be found.
  };

  Service(const Config service, const std::string& multicast_bind_address, const std::string& registry_multicast_address);
  Service(const Service&) = delete;
  Service(Service&&);
  ~Service();

  void set_service_announcment_callback(std::function<void(pfc_service_announcment&)>);

  auto name() const -> std::string;
  auto address() const -> std::string;
  auto brief() const -> std::string;
  auto port() const -> uint16_t;
  auto style() const -> Config::protocol;

  void start();
  void stop();
  void join();

  bool valid() const;
  Error error() const;

  Service& operator=(const Service&) = delete;
  Service& operator=(Service&&);

private:
  struct Implementation;
#pragma warning(suppress : 4251)
  std::unique_ptr<Implementation> _impl; //!< Pointer to Implementation
};
} //namespace pfc

#endif //SUSTAIN_SERVICE_H
