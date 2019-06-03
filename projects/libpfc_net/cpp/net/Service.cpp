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

#include <sustain/framework/net/Service.h>

#include <sustain/framework/Protocol.h>
#include <sustain/framework/net/Multicast_Receiver.h>
#include <sustain/framework/net/Multicast_Sender.h>
#include <sustain/framework/util/Error.h>

namespace pfc {
struct Service::Implementation {
  Implementation(const std::string& bind_address, const std::string& multicast_address);
  ~Implementation();
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = default;
  Implementation& operator=(const Implementation&) = delete;
  Implementation& operator=(Implementation&&) = delete;

  void announce_service_creation(std::ostream&);
  void handle_service_broadcasr_message(std::istream&);

  std::function<void(pfc_service_announcment&)> service_broadcast_callback;

  pfc_service_announcment service_config;
  Config::protocol style;

  Multicast_Sender multicast_announcment;
  Multicast_Receiver multicast_broadcast_listiner;
  pfc::Error server_status;
};
//-----------------------------------------------------------------------------
Service::Implementation::Implementation(const std::string& bind_address, const std::string& multicast_address)
  : multicast_announcment(multicast_address, g_pfc_registry_reg_port)
  , multicast_broadcast_listiner(bind_address, multicast_address, g_pfc_registry_announce_port)
{
}
//-----------------------------------------------------------------------------
Service::Implementation::~Implementation()
{
  multicast_announcment.stop();
  multicast_broadcast_listiner.stop();

  multicast_announcment.join();
  multicast_broadcast_listiner.join();
}
//-----------------------------------------------------------------------------
void Service::Implementation::announce_service_creation(std::ostream& os)
{
  service_config.serialize(os);
  std::cout << "Sending: " << service_config << "\n";
}
//-----------------------------------------------------------------------------
void Service::Implementation::handle_service_broadcasr_message(std::istream& is)
{
  pfc_service_announcment announcment;
  if (announcment.deserialize(is).is_ok()) {
    std::cout << "Registry Sent:" << announcment << "\n";
    if(service_broadcast_callback)
    { service_broadcast_callback(announcment); }
  }
}
//-----------------------------------------------------------------------------
Service::Service(const Config config, const std::string& multicast_bind_address, const std::string& registry_multicast_address)
  : _impl(std::make_unique<Implementation>(std::move(multicast_bind_address), std::move(registry_multicast_address)))
{
  pfc_service_announcment service;
  service._name = config.name;
  service._protacol = (config.style == Config::pub_sub) ? pfc_protocol::pub_sub : pfc_protocol::req_req;
  service._address = config.address.c_str();
  service._brief = config.brief;
  service._port = config.port;

  _impl->style = config.style;
  _impl->service_config = service;
}
//-----------------------------------------------------------------------------
Service::Service(Service&& obj)
  : _impl(std::move(obj._impl))
{
}
//-----------------------------------------------------------------------------
Service::~Service()
{
  stop();
  join();
}
//-----------------------------------------------------------------------------
void Service::start()
{
  using namespace std::placeholders;
  auto& impl = *_impl;
  _impl->multicast_announcment.async_send(std::bind(&Implementation::announce_service_creation, _impl.get(), _1));
  _impl->multicast_broadcast_listiner.async_receive(std::bind(&Implementation::handle_service_broadcasr_message, _impl.get(), _1));
}
//-----------------------------------------------------------------------------
void Service::stop()
{
  _impl->multicast_announcment.stop();
  _impl->multicast_broadcast_listiner.stop();
}
//-----------------------------------------------------------------------------
void Service::join()
{
  _impl->multicast_announcment.join();
  _impl->multicast_broadcast_listiner.join();
}
//-----------------------------------------------------------------------------
void Service::set_service_announcment_callback(std::function<void(pfc_service_announcment&)> func)
{
  _impl->service_broadcast_callback = func;
}
//-----------------------------------------------------------------------------
auto Service::name() const -> std::string
{
  return _impl->service_config._name;
}
//-----------------------------------------------------------------------------
auto Service::address() const -> std::string
{
  return _impl->service_config._address;
}
//-----------------------------------------------------------------------------
auto Service::brief() const -> std::string
{
  return _impl->service_config._brief;
}
//-----------------------------------------------------------------------------
auto Service::port() const -> uint16_t
{
  return _impl->service_config._port;
}
//-----------------------------------------------------------------------------
auto Service::style() const -> Config::protocol
{
  return _impl->style;
}
//-----------------------------------------------------------------------------
bool Service::valid() const
{
  return _impl->server_status.is_ok();
}
//-----------------------------------------------------------------------------
Error Service::error() const
{
  return _impl->server_status;
}
//-----------------------------------------------------------------------------
Service& Service::operator=(Service&& obj)
{
  _impl = std::move(obj._impl);
  return *this;
}
}
