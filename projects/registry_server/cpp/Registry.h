#ifndef SUSTAIN_REGISRY_H
#define SUSTAIN_REGISRY_H

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
//! \brief Defines a Service Registry Class
//!        Class meets the speficiation of the Sustain Framework
//!

#include <memory>
#include <string>

#include <sustain/framework/Exports.h>
#include <sustain/framework/util/Error.h>
#include <sustain/framework/util/Constants.h>

namespace pfc {
class Registry {
public:
  Registry(std::string bind_address, std::string multicast_address);
  Registry(const Registry&) = delete;
  Registry(Registry&&);
  ~Registry();

  void start();
  void wait();
  void shutdown();

  bool is_valid();
  Error error();

  Registry& operator=(const Registry&) = delete;
  Registry& operator=(Registry&&);

private:
  struct Implementation;
#pragma warning(suppress:4251)
  std::unique_ptr<Implementation> _impl;

};
} //namespace pfc

#endif //SUSTAIN_REGISRY_H
