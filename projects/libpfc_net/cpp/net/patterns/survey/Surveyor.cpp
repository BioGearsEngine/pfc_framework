#include <sustain/framework/net/patterns/survey/Surveyor.h>

namespace pfc {
Survey_Surveyor::~Survey_Surveyor() {}

void Survey_Surveyor::broadcast(std::function<void(void)>) {}
void Survey_Surveyor::async_broadcast(std::function<void(void)>) {}

void Survey_Surveyor::standup() {}
void Survey_Surveyor::shutdown() {}
}