#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_SUBSCRIBER_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_SUBSCRIBER_H

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
#include <sustain/framework/net/Uri.h>
#include <memory>

namespace pfc {
class SUSTAIN_FRAMEWORK_API PubSub_Subscriber : public Listiner {
  PubSub_Subscriber(URI);
  ~PubSub_Subscriber() final;

  void listen(ListenFunc) final;
  void async_listen(ListenFunc) final;

  void standup() final;
  void shutdown() final;

private:
#pragma warning(push, 0)
  struct Implementation;
  std::unique_ptr<Implementation> _impl;
#pragma warning(pop)
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_SUBSCRIBER_H
