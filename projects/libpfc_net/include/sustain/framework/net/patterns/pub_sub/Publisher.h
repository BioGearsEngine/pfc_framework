#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H

#include <sustain/framework/net/Patterns.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API PubSub_Publisher : public Broadcaster {
public:
  ~PubSub_Publisher() final;

  void broadcast(std::function<void(void)>) final;
  void async_broadcast(std::function<void(void)>) final;

  void standup() final;
  void shutdown() final;
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H