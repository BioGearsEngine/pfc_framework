#ifndef SUSTAIN_SERVICE_H
#define SUSTAIN_SERVICE_H

//!
//! \author Steven A White
//! \date   2019-02-14
//!
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

class SUSTAIN_FRAMEWORK_API Service {
public:
  struct Config {
    enum protocol { pub_sub,
                    req_rep,
                    survey,
    } style;
    uint16_t port;
    std::string name;
    URI address;
    std::string brief;
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
  std::unique_ptr<Implementation> _impl;
};
} //namespace pfc

#endif //SUSTAIN_SERVICE_H