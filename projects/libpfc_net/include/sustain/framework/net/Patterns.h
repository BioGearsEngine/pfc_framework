#ifndef SUSTAIN_FRAMEWORK_NET_PATTERNS_H
#define SUSTAIN_FRAMEWORK_NET_PATTERNS_H

#include <functional>

#include <sustain/framework/Exports.h>
namespace pfc
{
  class SUSTAIN_FRAMEWORK_API Listiner
  {
  public:
    virtual ~Listiner() = default;

    virtual void  listen( std::function<void(void) > ) = 0;
    virtual void  async_listen( std::function<void(void) > ) = 0;

    virtual void standup() = 0;
    virtual void shutdown() = 0;
  };

  class SUSTAIN_FRAMEWORK_API Broadcaster
  {
  public:
    virtual ~Broadcaster() = default;
    
    virtual void  broadcast( std::function<void(void) > ) = 0;
    virtual void  async_broadcast( std::function<void(void) > ) = 0;

    virtual void standup() = 0;
    virtual void shutdown() = 0;
  };

} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_H