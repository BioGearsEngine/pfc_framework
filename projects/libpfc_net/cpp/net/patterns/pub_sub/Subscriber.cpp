#include <sustain/framework/net/patterns/pub_sub/Subscriber.h>

#include <sustain/framework/util/Error.h>

namespace pfc {
PubSub_Subscriber::~PubSub_Subscriber() {}

void PubSub_Subscriber::listen(std::function<void(void)>) {}
void PubSub_Subscriber::async_listen(std::function<void(void)>) {}

void PubSub_Subscriber::standup() {}
void PubSub_Subscriber::shutdown() {}
}