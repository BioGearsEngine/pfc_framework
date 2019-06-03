#ifndef SUSTAIN_FRAMEWORK_NET_MULTICAST_SENDER_H
#define SUSTAIN_FRAMEWORK_NET_MULTICAST_SENDER_H


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
//!
//! \brief General Wapper Class for working with UDP/MULTI CAST


#include <memory>
#include <string>
#include <functional>

#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Error.h>
#include <sustain/framework/util/Constants.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API Multicast_Sender {
public:
  Multicast_Sender(std::string multicast_address, uint16_t port);
  Multicast_Sender(const Multicast_Sender&) = delete;
  Multicast_Sender(Multicast_Sender&&);
  ~Multicast_Sender();

  void send( std::function<void(std::ostream&)> );
  void async_send( std::function<void(std::ostream&)> );
  void join();
  void stop();

  bool is_valid();
  Error error() const;

  Multicast_Sender& operator=(const Multicast_Sender&) = delete;
  Multicast_Sender& operator=(Multicast_Sender&&);

private:
  struct Implementation;
#pragma warning(suppress:4251)
  std::unique_ptr<Implementation> _impl;

};
} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_MULTICAST_SENDER_H
