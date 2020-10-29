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

#include <sustain/framework/net/patterns/pub_sub/Publisher.h>

#include <iostream>
#include <thread>

#include <nanomsg/pubsub.h>

#include "../nanomsg_helper.h"

namespace pfc {

//!
//!  @struct nn_buffer
//!  Streambuf implementation to allow us to return nanomessage buffers as streams
//!
struct nn_buffer : std::streambuf {
  //! \param begin : Pointer to start of the streambuffer
  //! \param end  :  Pointer to end of the contigious memory buffer.
  //!
  //!  Converts two pointers to a serious of contiguous memory in to a nanomsg buffer
  nn_buffer(char* begin, char* end)
  {
    this->setg(begin, begin, end);
  }
};

//!
//!  @struct PubSub_Publisher::Implementation
//!  Private PIMPL implementation of PubSub_Publisher
//!
struct PubSub_Publisher::Implementation {
  Implementation(URI&&);
  ~Implementation();

  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  Implementation& operator==(const Implementation&) = delete;
  Implementation& operator==(Implementation&&) = delete;

  URI uri;               //!<  URI of the service to be given to nano_msg
  int socket;            //!<  Socket the service runs 
  int rv;                //!<  return value of any nano_msg calls
  char* msg_buffer;      //!<  msg_buffer nano_messages internal buffer
  bool running;          //!<  Run control for async threading

  void publish();

  std::thread pubsub_main_thread; //!< Thread control for async braodcast request. 
  BroadcastFunc generate_message_func; //!<  Callback for processing received broadcast

  Error ec; //!< Current Error code of the system else Success()
};
//-------------------------------------------------------------------------------
//! Deconstructor for Implementation - Shutsdown all pending nn activity and clears memory
PubSub_Publisher::Implementation::~Implementation()
{
  if(rv && socket)
  {
    nn_shutdown(socket,rv);
  }

  if(socket)
  {
    nn_close(socket);
  }
  if(msg_buffer)
  {
    nn_freemsg(msg_buffer);
    msg_buffer = nullptr;
  }
}
//-------------------------------------------------------------------------------
//!
//! URI based constructor
//! \param u [IN,OUT] Moves URI in to palce and stands up nn_socket and nn_bind
PubSub_Publisher::Implementation::Implementation(URI&& u)
  : uri(std::move(u))
  , socket(0)
  , rv(0)
  , msg_buffer(nullptr)
  , running(false)
{
  if ((socket = nn_socket(AF_SP, NN_PUB)) < 0) {
    ec = nano_to_Error(socket);
  }
  if ((rv = nn_bind(socket, uri.c_str())) < 0) {
    ec = nano_to_Error(rv);
  }
}
//-----------------------------------------------------------------------------
//!
//! broadcast function for controlling Pub/Sub paradigm.
//! Underlying Implementation is done in nanomsg
//!
void PubSub_Publisher::Implementation::publish()
{
  do {
    int bytes = 0;
    auto buffer = generate_message_func();
    if ((bytes = nn_send(socket, &buffer[0], buffer.size(), 0)) < 0) {
      ec = nano_to_Error(bytes);
    }
  } while (running);
}
//-----------------------------------------------------------------------------
//!  URI based constructor. Takes a copy of a URI to setup networking information
//! \param uri [IN]  Service configuration of the new PubSub_Publisher
PubSub_Publisher::PubSub_Publisher(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-----------------------------------------------------------------------------
//!  Shuts down async threading and fress all memory
PubSub_Publisher::~PubSub_Publisher()
{
  nn_close(_impl->socket);
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
void PubSub_Publisher::set_response_callaback_func(CallbackFunc func)
{
}
//-----------------------------------------------------------------------------
//!
//! \param func [IN] -- Function that will be called to generate broadcast message
//!
//! Blocking call for receiving a single message
void PubSub_Publisher::broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = false;
  _impl->publish();
}
//-----------------------------------------------------------------------------
//! \param func [IN] --  Function that will be called to generate broadcast message
//! Non Blocking listen request; Will continue to process inbound messages by calling the BroadcastFunc
//! Until running is set to false.
//! Future versions of async_broadcast may allow multiple parallel broadcast, but this is currently
//! Undefined behavior.
//!
void PubSub_Publisher::async_broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::publish, _impl.get());
}
//-----------------------------------------------------------------------------
//! Part of the Pattern interface stands up all IO not handled by the RIIA
void PubSub_Publisher::standup()
{
}
//-----------------------------------------------------------------------------
//! Terminates all pending IO will invalidate future broadcast events and should onyl be called
//! Before the Surveyor goes out of scope.
void PubSub_Publisher::shutdown()
{
  _impl->running = true;
  if (_impl->socket) {
    nn_close(_impl->socket);
    _impl->socket = 0;
  }
}
//-----------------------------------------------------------------------------
}
