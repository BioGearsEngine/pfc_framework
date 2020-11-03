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

#include <sustain/framework/net/Service.h>

#include <sustain/framework/Protocol.h>
#include <sustain/framework/net/Multicast_Receiver.h>
#include <sustain/framework/net/Multicast_Sender.h>
#include <sustain/framework/util/Error.h>

namespace pfc {

//!
//!  PIMPL Implementation of Service class
//!
struct Service::Implementation {
  Implementation(const std::string& bind_address, const std::string& multicast_address);
  ~Implementation();
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = default;      //!< Default Move Constructor
  Implementation& operator=(const Implementation&) = delete;
  Implementation& operator=(Implementation&&) = delete;

  void announce_service_creation(std::ostream&);
  void handle_service_broadcaster_message(std::istream&);

  std::function<void(pfc_service_announcement&)> service_broadcast_callback;    //!<Callback function that is called each time a new service is announced

  pfc_service_announcement service_config;            //!<Struct containing details that will be broadcasted over mulicast so other services and clients can subscribe
  Config::protocol style;                            //!<Configuration Style of the service implmentation

  Multicast_Sender multicast_announcement;            //!<periodically announces itself to the network
  Multicast_Receiver multicast_broadcast_listiner;   //!< Responds to the announcement of new services
  pfc::Error server_status; //!< Current Error code of the system else Success()
};

//!
//!  \param bind_address [IN] std::string -- The IP4/IP6 address the implmentation will bind to for multicast purposes
//!  \param multicast_address [IN] std::string -- The IP4/IP6 address the implmentation will broadcast. Port number is assumed by the specification
Service::Implementation::Implementation(const std::string& bind_address, const std::string& multicast_address)
  : multicast_announcement(multicast_address, g_pfc_registry_reg_port)
  , multicast_broadcast_listiner(bind_address, multicast_address, g_pfc_registry_announce_port)
{
}
//-----------------------------------------------------------------------------
//!  Deconstructor of a Service. Will stop all async multicast activity 
//!  Derived class is responsible for shutting down the  true service activty
Service::Implementation::~Implementation()
{
  multicast_announcement.stop();
  multicast_broadcast_listiner.stop();

  multicast_announcement.join();
  multicast_broadcast_listiner.join();
}
//-----------------------------------------------------------------------------
//!
//! Announce the service creation over the ostream channel.  Will normally be
//! called by the programs service manager, but can be called at any time by the
//! service owner
//! \param os [IN,OUT] Call back used to transmit service configuration through async_multicast
//!
void Service::Implementation::announce_service_creation(std::ostream& os)
{
  service_config.serialize(os);
  std::cout << "Sending: " << service_config << "\n";
}
//-----------------------------------------------------------------------------
//!
//! Service broadcast call back message.
//! \param is S[IN,OUT] -- Stream which the broadcast message will be received.
//!
//! If the derived class has set a service_braodcast_callback it will be executed
//! for the derived class to handle the inbound config. This allows services to depend on each other
//! Before being fully live. 
//!
void Service::Implementation::handle_service_broadcaster_message(std::istream& is)
{
  pfc_service_announcement announcement;
  if (announcement.deserialize(is).is_ok()) {
    std::cout << "Registry Sent:" << announcement << "\n";
    if(service_broadcast_callback)
    { service_broadcast_callback(announcement); }
  }
}
//-----------------------------------------------------------------------------
//!
//!  Service Constructor
//!  \param config [IN] Config -- User provided configuration of how the derived service will run
//!  \param multicast_bind_address [IN] std::string -- Bind address on the system that the multicast will occur. Allows user to select specific NICs to send out multicast information on
//!  \param registry_multicast_address [IN] std::string -- Multicast channel broadcast will be sent on
Service::Service(const Config config, const std::string& multicast_bind_address, const std::string& registry_multicast_address)
  : _impl(std::make_unique<Implementation>(std::move(multicast_bind_address), std::move(registry_multicast_address)))
{
  pfc_service_announcement service;
  service._name = config.name;
  service._protacol = (config.style == Config::pub_sub) ? pfc_protocol::pub_sub : pfc_protocol::req_req;
  service._address = config.address.c_str();
  service._brief = config.brief;
  service._port = config.port;

  _impl->style = config.style;
  _impl->service_config = service;
}
//-----------------------------------------------------------------------------
//! Move constructor for a service
//! \param obj [IN] source object
Service::Service(Service&& obj)
  : _impl(std::move(obj._impl))
{
}
//-----------------------------------------------------------------------------
//! Deconstructor for a service
Service::~Service()
{
  stop();
  join();
}
//-----------------------------------------------------------------------------
//!  Runnable interface starts all multiservice activity and asyncronous threading.
void Service::start()
{
  using namespace std::placeholders;
  auto& impl = *_impl;
  _impl->multicast_announcement.async_send(std::bind(&Implementation::announce_service_creation, _impl.get(), _1));
  _impl->multicast_broadcast_listiner.async_receive(std::bind(&Implementation::handle_service_broadcaster_message, _impl.get(), _1));
}
//-----------------------------------------------------------------------------
//!  Runnable interface stops all multiservice activity and asyncronous threading.
void Service::stop()
{
  _impl->multicast_announcement.stop();
  _impl->multicast_broadcast_listiner.stop();
}
//-----------------------------------------------------------------------------
//!  Blocking call that will not return until multicast_announcement and multicast_braodcast_listner have  been 
void Service::join()
{
  _impl->multicast_announcement.join();
  _impl->multicast_broadcast_listiner.join();
}
//-----------------------------------------------------------------------------
//! Sets the callback function that qill be executed once per each service broadcast. void(void)
//! \param func [IN] stores a std::function to be executed as a callback
void Service::set_service_announcement_callback(std::function<void(pfc_service_announcement&)> func)
{
  _impl->service_broadcast_callback = func;
}
//-----------------------------------------------------------------------------
//! \return the service Name
//! 
auto Service::name() const -> std::string
{
  return _impl->service_config._name;
}
//-----------------------------------------------------------------------------
//!  \return the service address
//!
auto Service::address() const -> std::string
{
  return _impl->service_config._address;
}
//-----------------------------------------------------------------------------
//!  \return Returns the current brief
//!
//!  \brief A service brief should include expectations of how other services should use the
//!         Service in a general summary about the service
//!
auto Service::brief() const -> std::string
{
  return _impl->service_config._brief;
}
//-----------------------------------------------------------------------------
//! \return the current port of the service
auto Service::port() const -> uint16_t
{
  return _impl->service_config._port;
}
//-----------------------------------------------------------------------------
//! \return What Protocol style does this service use
auto Service::style() const -> Config::protocol
{
  return _impl->style;
}
//-----------------------------------------------------------------------------
//! \return is the service currently valid for use
bool Service::valid() const
{
  return _impl->server_status.is_ok();
}
//-----------------------------------------------------------------------------
//! \return The last error status of the service PFC::NO_ERROR if none has occured
Error Service::error() const
{
  return _impl->server_status;
}
//-----------------------------------------------------------------------------
//! Move opeerator for a service
Service& Service::operator=(Service&& obj)
{
  _impl = std::move(obj._impl);
  return *this;
}
}
