#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_CLIENT_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_CLIENT_H

#include <sustain/framework/net/Patterns.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API ReqRep_Client : public Broadcaster {
  ~ReqRep_Client() final;

  void broadcast(std::function<void(void)>) final;
  void async_broadcast(std::function<void(void)>) final;

  void standup() final;
  void shutdown() final;
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_CLIENT_H