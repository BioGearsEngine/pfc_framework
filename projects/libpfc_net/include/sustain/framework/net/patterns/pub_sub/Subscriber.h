#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_SUBSCRIBER_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_SUBSCRIBER_H

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