#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_SURVEYOR_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_SURVEYOR_H

#include <sustain/framework/net/Patterns.h>

#include <sustain/framework/net/Uri.h>
namespace pfc {
class SUSTAIN_FRAMEWORK_API Survey_Surveyor : public Broadcaster {

  Survey_Surveyor(URI);
  ~Survey_Surveyor() final;

  void set_response_callaback_func(CallbackFunc) final;
  void broadcast(BroadcastFunc) final;
  void async_broadcast(BroadcastFunc) final;

  void standup() final;
  void shutdown() final;

  private:
#pragma warning(push, 0)
  struct Implementation;
  std::unique_ptr<Implementation> _impl;
#pragma warning(pop)
};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_SURVEY_SURVEYOR_H