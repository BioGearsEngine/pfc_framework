#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_SERVER_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_REQREP_SERVER_H

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
#include <cstdint>
#include <string>

#include <sustain/framework/net/Uri.h>

namespace pfc {
//!
//! This class creates a Server to a Req/Rep style service
//! <a href="https://nanomsg.org/gettingstarted/nng/reqrep.htmll"> Documentation </a>
//!
//! Servers are traditional TCP/IP http response based implementations. 
//! They can receive multiple messages from multiple clients and will dispatch appropriate
//! responses in the order they are processed. Timeouts can occur if the server is to busy

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
