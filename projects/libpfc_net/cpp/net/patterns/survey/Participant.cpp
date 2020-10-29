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

#include <sustain/framework/net/patterns/survey/Participant.h>

#include <iostream>
#include <thread>

#include <nanomsg/survey.h>
#include "../nanomsg_helper.h"

namespace pfc {

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
//! PIMPL Implementation for a Survey_Participant
//!
struct Survey_Participant::Implementation {
  Implementation(URI&&);
  ~Implementation();

  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  Implementation& operator==(const Implementation&) = delete;
  Implementation& operator==(Implementation&&) = delete;

  URI uri;                   //!<  URI of the service to be given to nano_msg
  int socket;                //!<  Socket the service runs 
  int rv;                    //!<  return value of any nano_msg calls
  char* msg_buffer;          //!<  msg_buffer nano_messages internal buffer
  bool running;              //!<  Run control for async threading

  void listen();

  std::thread pubsub_main_thread; //!< Threading control or async read/write
  ListenFunc handle_message_func; //!< Function used to handle inbound messages

  Error ec; //!< Current Error code of the system else Success()
};
//-------------------------------------------------------------------------------
//!
//! URI based constructor
//! \param u [IN,OUT] Moves URI in to palce and stands up nn_socket and nn_bind
Survey_Participant::Implementation::Implementation(URI&& u)
  : uri(std::move(u))
  , socket(0)
  , rv(0)
  , msg_buffer(nullptr)
  , running(false)
{
  if ((socket = nn_socket(AF_SP, NN_RESPONDENT)) < 0) {
    ec = nano_to_Error(socket);
  }
  if (rv = nn_connect(socket, uri.c_str()) < 0) {
    ec = nano_to_Error(socket);
  }
}
    //-------------------------------------------------------------------------------
//! Deconstructor for Implementation - Shutsdown all pending nn activity and clears memory
Survey_Participant::Implementation::~Implementation()
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
void Survey_Participant::Implementation::listen()
{
  do {
    char* l_buffer = NULL;
    int bytes;
    if ((bytes = nn_recv(socket, &l_buffer, NN_MSG, 0)) < 0) {
      ec = nano_to_Error(bytes);
      continue;
    }
    std::vector<char> response = handle_message_func(l_buffer, bytes);
    nn_freemsg(l_buffer);
    if ((bytes = nn_send(socket, response.data(), response.size(), 0)) < 0) {
      ec = nano_to_Error(bytes);
    }
  } while (running);
}
//-----------------------------------------------------------------------------
//!  URI based constructor. Takes a copy of a URI to setup networking information
//! \param URI [IN]  Service configuration of the new Surveyor
Survey_Participant::Survey_Participant(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-----------------------------------------------------------------------------
//!  Shuts down async threading and fress all memory
Survey_Participant::~Survey_Participant()
{
  nn_close(_impl->socket);
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
//
// \param func [IN] Function to use to react to a received message
//
//Blocks until message is received.
void Survey_Participant::listen(ListenFunc func)
{
  _impl->handle_message_func = func;
  _impl->running = false;
  _impl->listen();
}
//-----------------------------------------------------------------------------
//
// \param func [IN] Function to use to generate Survey broadcast message
//
// Non Blocking broadcast request that will launch a thread and execute the send.
// This message should not be called until the ListenFunc has excuted and the thread joined.
// Future versions of async_listen may allow multiple parallel broadcast, but this is currently
// Undefined behavior.
//
// Thead implementation will respond to multiple consecutive messages until stop is called
void Survey_Participant::async_listen(ListenFunc func)
{
  _impl->handle_message_func = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-----------------------------------------------------------------------------
//! Part of the Pattern interface stands up all IO not handled by the RIIA
//
void Survey_Participant::standup()
{
}
//-----------------------------------------------------------------------------
//Terminates all pending IO will invalidate future broadcast events and should onyl be called
//Before the Surveyor goes out of scope.
void Survey_Participant::shutdown()
{
  _impl->running = true;
  if (_impl->socket) {
    nn_close(_impl->socket);
    _impl->socket = 0;
  }
}
//-----------------------------------------------------------------------------
}
