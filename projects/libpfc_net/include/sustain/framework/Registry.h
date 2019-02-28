#ifndef SUSTAIN_REGISRY_H
#define SUSTAIN_REGISRY_H

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
#include <sustain/framework/Error.h>
#include <sustain/framework/Constants.h>

namespace pfc {
class SUSTAIN_FRAMEWORK_API Registry {
public:
  Registry(std::string bind_address, std::string multicast_address);
  Registry(const Registry&) = delete;
  Registry(Registry&&);
  ~Registry();

  void start();
  void stop();
  void join();

  bool Valid();
  Error Error();

  Registry& operator=(const Registry&) = delete;
  Registry& operator=(Registry&&);

private:
  struct Implementation;
#pragma warning(suppress:4251)
  std::unique_ptr<Implementation> _impl;

};
} //namespace pfc

#endif //SUSTAIN_REGISRY_H