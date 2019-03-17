#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_SUBSCRIBER_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_SUBSCRIBER_H

#include <sustain/framework/net/Patterns.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API PubSub_Subscriber : public Listiner {
  ~PubSub_Subscriber() final;

  void listen(std::function<void(void)>) final;
  void async_listen(std::function<void(void)>) final;

  void standup() final;
  void shutdown() final;
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_SUBSCRIBER_H