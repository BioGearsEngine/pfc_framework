#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_CLIENT_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_CLIENT_H

#include <sustain/framework/net/Patterns.h>

#include <sustain/framework/net/Uri.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API ReqRep_Client : public Broadcaster {
public:
  ReqRep_Client(URI);
  ~ReqRep_Client() final;

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

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_CLIENT_H