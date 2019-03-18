#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H

#include <sustain/framework/net/Patterns.h>

#include <memory>

#include <sustain/framework/net/Uri.h>

namespace pfc {
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
  struct Implementation;
  std::unique_ptr<Implementation> _impl;
#pragma warning(pop)
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_PUBSUB_PUBLISHER_H