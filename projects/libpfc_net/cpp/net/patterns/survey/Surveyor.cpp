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

#include <sustain/framework/net/patterns/survey/Surveyor.h>

#include <iostream>
#include <thread>

#include "../nanomsg_helper.h"
#include <boost/log/trivial.hpp>
#include <nanomsg/survey.h>

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
//!  PIMPL Implementation of Survey_Surveyor
//!
struct Survey_Surveyor::Implementation {
  Implementation(URI&&);
  ~Implementation();

  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  Implementation& operator==(const Implementation&) = delete;
  Implementation& operator==(Implementation&&) = delete;

  URI uri;                //!<  URI of the service to be given to nano_msg
  int socket;             //!<  Socket the service runs 
  int rv;                 //!<  return value of any nano_msg calls
  char* msg_buffer;       //!<  msg_buffer nano_messages internal buffer
  bool running;           //!<  Run control for async threading

  void listen();

  std::thread pubsub_main_thread;         //!< Threading control or async read/write
  BroadcastFunc generate_message_func;    //!< Function used to generate message needed for call back
  CallbackFunc response_callback_function; //!< Function used to respond to message used as callback in nanomssg

  Error ec;  //!< Current Error code of the system else Success()
};
//-------------------------------------------------------------------------------
//!
//! URI based constructor 
//! \param u [IN,OUT] Moves URI in to palce and stands up nn_socket and nn_bind
Survey_Surveyor::Implementation::Implementation(URI&& u)
  : uri(std::move(u))
  , socket(0)
  , rv(0)
  , msg_buffer(nullptr)
  , running(false)
  , ec(Success())
{
  if ((socket = nn_socket(AF_SP, NN_SURVEYOR)) < 0) {
    ec = nano_to_Error(socket);
  }
  if (nn_bind(socket, uri.c_str()) < 0) {
    ec = nano_to_Error(socket);
  }
}
//-------------------------------------------------------------------------------
//! Deconstructor for Implementation - Shutsdown all pending nn activity and clears memory
Survey_Surveyor::Implementation::~Implementation()
{
  if (rv && socket) {
    nn_shutdown(socket, rv);
  }

  if (socket) {
    nn_close(socket);
  }
  if (msg_buffer) {
    nn_freemsg(msg_buffer);
    msg_buffer = nullptr;
  }
}
//-----------------------------------------------------------------------------
//!
//! Listen function for controlling Survery paradigm.
//! Underlying Implementation is done in nanomsg
//!
void Survey_Surveyor::Implementation::listen()
{
  do {
    int bytes = 0;
    auto buffer = generate_message_func();
    if ((bytes = nn_send(socket, &buffer[0], buffer.size(), 0)) < 0) {
      ec = nano_to_Error(bytes);
      continue;
    }

    for (;;) {
      char* buf = NULL;
      int bytes = nn_recv(socket, &buf, NN_MSG, 0);
      if (bytes < 0) {
        if (nn_errno() == ETIMEDOUT) {
          break;
        }
        ec = nano_to_Error(bytes);
        continue;
      }
      if (response_callback_function) {
        nn_buffer stream_buffer{ buf, buf + bytes };
        std::istream message_stream{ &stream_buffer };

        response_callback_function(message_stream);
      }
      nn_freemsg(buf);
    }

  } while (running);
}
//-----------------------------------------------------------------------------
//!  URI based constructor. Takes a copy of a URI to setup networking information
//! \param uri [IN]  Service configuration of the new Surveyor
Survey_Surveyor::Survey_Surveyor(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-----------------------------------------------------------------------------
//!  Shuts down async threading and frees all memory
Survey_Surveyor::~Survey_Surveyor()
{
  nn_close(_impl->socket);
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
//! /param func [IN] Callback function to be called when a Particpant responds to a survey
//!
//! Allows API users to set arbitary lambdas to be executed when a survey is complete
void Survey_Surveyor::set_response_callaback_func(CallbackFunc func)
{
  _impl->response_callback_function = func;
}
//-----------------------------------------------------------------------------
//! 
//!  \param func [IN] Function to use to generate Survey broadcast message
//! 
//! Blocking call to a single broadcast. Broadcast Func should return a valid seralized braodcast message that particpants will be able to deserialize. 
void Survey_Surveyor::broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = false;
  _impl->listen();
}
//-----------------------------------------------------------------------------
//!
//! \param func [IN] Function to use to generate Survey broadcast message
//!
//! Non Blocking broadcast request that will launch a thread and execute the send.
//! This message should not be called until the BroadcastFunc has excuted and the thread joined.
//! Future versions of async_broadcast may allow multiple parallel broadcast, but this is currently
//! Undefined behavior.
//! 
//! Thread implementaiton will continue to respond to broadcast until the first
//! Timeout occurs Then rebroad cast message until running is set to false
void Survey_Surveyor::async_broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-----------------------------------------------------------------------------
//! Part of the Pattern interface stands up all IO not handled by the RIIA
//
void Survey_Surveyor::standup()
{
}
//-----------------------------------------------------------------------------
//! Terminates all pending IO will invalidate future broadcast events and should onyl be called
//! Before the Surveyor goes out of scope.
void Survey_Surveyor::shutdown()
{
  _impl->running = false;
  if (_impl->socket) {
    nn_close(_impl->socket);
    _impl->socket = 0;
  }
}
//-----------------------------------------------------------------------------
}
