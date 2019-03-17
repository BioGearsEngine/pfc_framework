#ifndef SUSTAIN_FRAMEWORK_NET_MULTICAST_RECEIVER_H
#define SUSTAIN_FRAMEWORK_NET_MULTICAST_RECEIVER_H

//!
//! \author Steven A White
//! \date   2019-02-14
//!
//! \brief General Wapper Class for working with UDP/MULTI CAST


#include <memory>
#include <string>
#include <functional>

#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Error.h>
#include <sustain/framework/util/Constants.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API Multicast_Receiver {
public:
  Multicast_Receiver(std::string bind_address, std::string multicast_address, uint16_t port);
  Multicast_Receiver(const Multicast_Receiver&) = delete;
  Multicast_Receiver(Multicast_Receiver&&);
  ~Multicast_Receiver();

  void receive( std::function<void(std::istream&)> );
  void async_receive( std::function<void(std::istream&)> );
  void join();
  void stop();

  size_t buffer_legth() const;
  void buffer_legth( const size_t);

  bool is_valid();
  Error error();

  Multicast_Receiver& operator=(const Multicast_Receiver&) = delete;
  Multicast_Receiver& operator=(Multicast_Receiver&&);

private:
  struct Implementation;
#pragma warning(suppress:4251)
  std::unique_ptr<Implementation> _impl;

};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_MULTICAST_RECEIVER_H