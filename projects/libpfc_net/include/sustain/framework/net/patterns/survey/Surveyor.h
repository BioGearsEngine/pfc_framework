#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_SURVEYOR_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_SURVEYOR_H

#include <sustain/framework/net/Patterns.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API Survey_Surveyor : public Broadcaster {
  ~Survey_Surveyor() final;

  void broadcast(std::function<void(void)>) final;
  void async_broadcast(std::function<void(void)>) final;

  void standup() final;
  void shutdown() final;
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_SURVEYOR_H