#include <sustain/framework/net/patterns/req_rep/Server.h>

namespace pfc {
ReqRep_Server::~ReqRep_Server() {}

void ReqRep_Server::listen(std::function<void(void)>) {}
void ReqRep_Server::async_listen(std::function<void(void)>) {}

void ReqRep_Server::standup() {}
void ReqRep_Server::shutdown() {}
}