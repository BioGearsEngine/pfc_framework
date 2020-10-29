#ifndef SUSTAIN_FRAMEWORK_NET_MULTICAST_RECEIVER_H
#define SUSTAIN_FRAMEWORK_NET_MULTICAST_RECEIVER_H

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

//!
//! \author Steven A White
//! \date   2019-02-14
//! \file
//! \brief General Wapper Class for working with UDP/MULTI CAST


#include <memory>
#include <string>
#include <functional>

#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Error.h>
#include <sustain/framework/util/Constants.h>

namespace pfc {

/**
 * Multicast receiver is designed ti simplify integration  with a multi cast service. 
 * Implementation utilizes boost_asio
*/
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
  /** @struct Implementation
 * Multicast_Receiver PIMPL Implementation Struct
 *
 */
  struct Implementation;
#pragma warning(suppress:4251)
  std::unique_ptr<Implementation> _impl;

};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_MULTICAST_RECEIVER_H
