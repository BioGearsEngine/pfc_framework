#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_PARTICIPANT_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_PARTICIPANT_H

#include <sustain/framework/net/Patterns.h>

#include <memory>

#include <sustain/framework/net/Uri.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API Survey_Participant : public Listiner {

  Survey_Participant(URI);
  ~Survey_Participant() final;

  void listen(ListenFunc) final;
  void async_listen(ListenFunc) final;

  void standup() final;
  void shutdown() final;

private:
#pragma warning(push, 0)
  struct Implementation;
  std::unique_ptr<Implementation> _impl;
#pragma warning(pop)
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_PARTICIPANT_H