#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_SERVER_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_SERVER_H

#include <sustain/framework/net/Patterns.h>

#include <memory>
#include <cstdint>
#include <string>

#include <sustain/framework/net/Uri.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API ReqRep_Server : public Listiner {
public:

  ReqRep_Server(URI);
  ~ReqRep_Server() final;

  void listen(ListenFunc) final;
  void async_listen(ListenFunc) final;

  void standup() final;
  void shutdown() final;
private:
#pragma warning(push,0)
  struct Implementation;
  std::unique_ptr<Implementation> _impl;
#pragma warning(pop)
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_SERVER_H