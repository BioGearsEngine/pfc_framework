/**************************************************************************************
Copyright 2019 Applied Research Associates, Inc.
Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License. You may obtain a copy of the License
at:
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
**************************************************************************************/

#include "Registry.h"

#include <mutex>
#include <queue>
#include <unordered_map>

#include <sustain/framework/Protocol.h>
#include <sustain/framework/net/Multicast_Receiver.h>
#include <sustain/framework/net/Multicast_Sender.h>
#include <sustain/framework/util/Constants.h>

namespace pfc {
struct Registry::Implementation {
  Implementation(std::string& bind_address, std::string& multicast_address);
  ~Implementation();
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = default;
  Implementation& operator=(const Implementation&) = delete;
  Implementation& operator=(Implementation&&) = default;

  void process_subscription_message(std::istream&);
  void broadcast_service_subscription(std::ostream&);

  Multicast_Receiver subscription_listiner;
  Multicast_Sender service_broadcaster;

  std::mutex broadcast_mutex;
  std::condition_variable broadcast_condition;

  std::unordered_map<std::string, pfc_service_announcement> services;
  std::queue<std::string> pending_broadcast;
};
//-----------------------------------------------------------------------------
Registry::Implementation::Implementation(std::string& bind_address, std::string& multicast_address)
  : subscription_listiner(bind_address, multicast_address, g_pfc_registry_reg_port)
  , service_broadcaster(multicast_address, g_pfc_registry_announce_port)
{
}
//-----------------------------------------------------------------------------
Registry::Implementation::~Implementation()
{
  subscription_listiner.stop();
  service_broadcaster.stop();
}
//-----------------------------------------------------------------------------
void Registry::Implementation::process_subscription_message(std::istream& is)
{
  pfc_service_announcement message;
  if (message.deserialize(is).is_ok()) {
    auto key = message._address + ":" + std::to_string(message._port);
    std::cout << "Received: " << message << "\n";
    //TODO: We likely need to move towards more conccurent_safe data structures
    //      So we can remove these locks
    broadcast_mutex.lock();
    services[key] = message;
    pending_broadcast.push(key);
    broadcast_mutex.unlock();

    service_broadcaster.send(std::bind(&Implementation::broadcast_service_subscription, this, std::placeholders::_1));
  } else {
  }
}
//-----------------------------------------------------------------------------
void Registry::Implementation::broadcast_service_subscription(std::ostream& os)
{

  if (!pending_broadcast.empty()) {
    pfc_service_announcement service = services[pending_broadcast.front()];
    pending_broadcast.pop();

    if (service.serialize(os).is_not_ok()) {
      os.clear();
    }
  }
}
//-----------------------------------------------------------------------------
Registry::Registry(std::string bind_address, std::string multicast_address)
  : _impl(std::make_unique<Implementation>(bind_address, multicast_address))
{
}
//-----------------------------------------------------------------------------
Registry::Registry(Registry&& obj)
  : _impl(std::move(obj._impl))
{
}
//-----------------------------------------------------------------------------
Registry::~Registry()
{
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
void Registry::start()
{
  using namespace std::placeholders;
  _impl->subscription_listiner.async_receive(std::bind(&Implementation::process_subscription_message, _impl.get(), _1));
}
void Registry::wait()
{
  _impl->subscription_listiner.join();
  _impl->service_broadcaster.join();
}
//-----------------------------------------------------------------------------
void Registry::shutdown()
{
  _impl->subscription_listiner.stop();
  _impl->service_broadcaster.stop();

  _impl->subscription_listiner.join();
  _impl->service_broadcaster.join();
}

//-----------------------------------------------------------------------------
bool Registry::is_valid()
{
  return _impl->subscription_listiner.is_valid() && _impl->service_broadcaster.is_valid();
}
//-----------------------------------------------------------------------------
Error Registry::error()
{
  return _impl->subscription_listiner.error() | _impl->service_broadcaster.error();
}
//-----------------------------------------------------------------------------
Registry& Registry::operator=(Registry&& obj)
{
  _impl = std::move(obj._impl);
  return *this;
}
}
