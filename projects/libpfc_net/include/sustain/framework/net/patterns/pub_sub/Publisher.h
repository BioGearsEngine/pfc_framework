#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H

/*! \file */ 

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

#include <sustain/framework/net/Patterns.h>

#include <memory>

#include <sustain/framework/net/Uri.h>

namespace pfc {

//!
//! Publisher class for Pub/Sub mechanics
//! A publisher is a source of messages that a
//! subscriber will register with to receive publushed events.
//! Follows the Broadcaster interface
//!
class SUSTAIN_FRAMEWORK_API PubSub_Publisher : public Broadcaster {
public:
  PubSub_Publisher(URI);
  ~PubSub_Publisher() final;

  void set_response_callaback_func(CallbackFunc) final;

  void broadcast(BroadcastFunc) final;
  void async_broadcast(BroadcastFunc) final;

  void standup() final;
  void shutdown() final;

private:
#pragma warning(push, 0)

//!
  //!  @struct PubSub_Publisher::Implementation
  //!  Private PIMPL implementation of PubSub_Publisher
  //!
  struct Implementation;
  std::unique_ptr<Implementation> _impl;
#pragma warning(pop)
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H
