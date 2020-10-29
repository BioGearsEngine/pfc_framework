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

/*! \file */

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <vector>

#include <sustain/framework/Exports.h>

namespace pfc {


//!
//! Abstract interface Pattern defines
//! Shutdown and Standup interface requirments for all
//! Listeners and Broadcasters
//!
class SUSTAIN_FRAMEWORK_API Pattern {
public:
  virtual ~Pattern() = default; //!< Default Deconstructor

  //! Must be called before using any pattern to setup all required devices
  virtual void standup() = 0;

  //! Must be called before destroying any Pattern to release any devices aquired at standup.
  //! May be blocking, but should automatically terminate any control variables in associated threads. 
  virtual void shutdown() = 0;
};

//!
//! Inherits from Patternes
//! Interface for any interface which listens on a socket until a message is received
//! defines listen and async_listen
//!
class SUSTAIN_FRAMEWORK_API Listiner : public Pattern {
public:

  //! \brief lambda signiture for functions used by listen and async_listen
  using ListenFunc = std::function<std::vector<char>(char*, size_t size)>;

  ~Listiner() override = default;

  //! \brief Blocking listen on underpending impementation. control will not return until timeout or a message is received
  virtual void listen(ListenFunc) = 0;

  //! \brief non blocking listen that will execute ListenFunc once a message is received
  virtual void async_listen(ListenFunc) = 0;

  //! \brief Part of the Pattern Interface
  void standup() override = 0;

  //! \brief Part of the Pattern Interface
  void shutdown() override = 0;
};

//!
//! Inherits from Pattern
//! 
//! Broadcaster is any class which can send a message without requring it first receive a message
//! Includes teh ability to listen on broadcast for responses.
//!
class SUSTAIN_FRAMEWORK_API Broadcaster : public Pattern {
public:
  //! \brief Standard Callback function for generating outbound message buffers
  //!
  //! The PFC Framework uses std::vectors as its underlying buffer to encourage simple allocation,de-alloation behavior.
  //! If a user is worried about allocation/deallocaiton performance they should overload the vectors  allocator type inside the
  //! Lambda  The return of this function  should always be a PFC seralized message format
  using BroadcastFunc = std::function<std::vector<char>()>;
  //! \brief Standard Callback for handeling received messages on a bounded port
  //!
  //!  This function will take in a istream which contains an arbitary amount of data and consume the message returning
  //!  no error codes. As this lambda will excute in the context of a user defined class. using std::bind for creating some
  //~  closure should sufficient for retaining state of the functions operations.
  using CallbackFunc = std::function<void(std::istream&)>;

  ~Broadcaster() override = default;          //!< Virtual Deconstructor

  virtual void set_response_callaback_func(CallbackFunc) = 0;    //!< Interface function for setting the Broadcaster Callback function

  virtual void broadcast(BroadcastFunc) = 0;       //!< Interface for performing syncronized braodcast
  virtual void async_broadcast(BroadcastFunc) = 0; //!< Interface for performing asyncronized broadcast


  void standup() override = 0;          //!< Inherited from Pattern
  void shutdown() override = 0;        //!< Inherited from Pattern

};

} //namespace pfc

#endif //SUSTAIN_FRAMEWORK_NET_PATTERNS_H
