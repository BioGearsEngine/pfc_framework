#include <sustain/framework/net/patterns/req_rep/Client.h>

namespace pfc {
ReqRep_Client::~ReqRep_Client() {}

void ReqRep_Client::broadcast(std::function<void(void)>) {}
void ReqRep_Client::async_broadcast(std::function<void(void)>) {}

void ReqRep_Client::standup() {}
void ReqRep_Client::shutdown() {}
}