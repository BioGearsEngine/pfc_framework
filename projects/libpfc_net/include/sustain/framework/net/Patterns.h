#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_H

/**************************************************************************************
copyright 2019 applied research associates, inc.
licensed under the apache license, version 2.0 (the "license"); you may not use
this file except in compliance with the license. you may obtain a copy of the license
at:
http://www.apache.org/licenses/license-2.0
unless required by applicable law or agreed to in writing, software distributed under
the license is distributed on an "as is" basis, without warranties or
conditions of any kind, either express or implied. see the license for the
specific language governing permissions and limitations under the license.
**************************************************************************************/


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
