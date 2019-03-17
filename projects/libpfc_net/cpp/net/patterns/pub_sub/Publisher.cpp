#include <sustain/framework/net/patterns/pub_sub/Publisher.h>

namespace pfc {
PubSub_Publisher::~PubSub_Publisher() {}

void PubSub_Publisher::broadcast(std::function<void(void)>) {}
void PubSub_Publisher::async_broadcast(std::function<void(void)>) {}

void PubSub_Publisher::standup() {}
void PubSub_Publisher::shutdown() {}
}