#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_H

#include <cstdint>
#include <functional>
#include <vector>

#include <sustain/framework/Exports.h>

namespace pfc {

class SUSTAIN_FRAMEWORK_API Pattern {
public:
  virtual ~Pattern() = default;

  virtual void standup() = 0;
  virtual void shutdown() = 0;
};

class SUSTAIN_FRAMEWORK_API Listiner : public Pattern {
public:
  using ListenFunc = std::function<std::vector<char>(char*, size_t size)>;

  ~Listiner() override = default;

  virtual void listen(ListenFunc) = 0;
  virtual void async_listen(ListenFunc) = 0;

  void standup() override = 0;
  void shutdown() override = 0;
};

class SUSTAIN_FRAMEWORK_API Broadcaster : public Pattern {
public:
  using BroadcastFunc = std::function<std::vector<char>()>;
  using CallbackFunc = std::function<void(std::istream&)>;

  ~Broadcaster() override = default;

  virtual void set_response_callaback_func(CallbackFunc) = 0;

  virtual void broadcast(BroadcastFunc) = 0;
  virtual void async_broadcast(BroadcastFunc) = 0;

  void standup() override = 0;
  void shutdown() override = 0;

};

} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_H