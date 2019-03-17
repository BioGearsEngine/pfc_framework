#include <sustain/framework/net/patterns/survey/Participant.h>

namespace pfc {
Survey_Participant::~Survey_Participant() {}

void Survey_Participant::listen(std::function<void(void)>) {}
void Survey_Participant::async_listen(std::function<void(void)>) {}

void Survey_Participant::standup() {}
void Survey_Participant::shutdown() {}
}