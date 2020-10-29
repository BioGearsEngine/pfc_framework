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

#include <sustain/framework/net/patterns/req_rep/Server.h>

#include <thread>

#include <nanomsg/reqrep.h>

#include "../nanomsg_helper.h"
#include "sustain/framework/net/patterns/survey/Surveyor.h"

#include <sustain/framework/util/Error.h>
namespace pfc {
//!
//! PIMPL Implementation for a ReqRep_Server
//!
struct ReqRep_Server::Implementation {
  Implementation(URI&&);
  ~Implementation();

  URI uri; //!<  URI of the service to be given to nano_msg
  int socket; //!<  Socket the service runs
  int rv; //!<  return value of any nano_msg calls
  char* msg_buffer; //!<  msg_buffer nano_messages internal buffer
  bool running; //!<  Run control for async threading

  void listen();

  std::thread pubsub_main_thread; //!< Threading control for async read/writes
  ListenFunc message_process_function; //!< Call back functions for managing

  Error ec; //!< Current Error code of the system else Success()
};
//-------------------------------------------------------------------------------
//!
//! URI based constructor
//! \param u [IN,OUT] Moves URI in to palce and stands up nn_socket and nn_bind
ReqRep_Server::Implementation::Implementation(URI&& u)
  : uri(std::move(u))
  , socket(0)
  , rv(0)
  , msg_buffer(nullptr)
  , running(false)
{
  if ((socket = nn_socket(AF_SP, NN_REP)) < 0) {
    ec = nano_to_Error(socket);
  }
  if ((rv = nn_bind(socket, uri.c_str())) < 0) {
    ec = nano_to_Error(rv);
  }
}
//-------------------------------------------------------------------------------
//! Deconstructor for Implementation - Shutsdown all pending nn activity and clears memory
ReqRep_Server::Implementation::~Implementation()
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
//-------------------------------------------------------------------------------
//!
//! Listen function for controlling Surver paradigm.
//! Underlying Implementation is done in nanomsg
//!
void ReqRep_Server::Implementation::listen()
{
  do {
    char* l_buffer = NULL;
    int bytes;
    if ((bytes = nn_recv(socket, &l_buffer, NN_MSG, 0)) < 0) {
      ec = nano_to_Error(bytes);
      continue;
    }
    std::vector<char> response = message_process_function(l_buffer, bytes);
    if ((bytes = nn_send(socket, response.data(), response.size(), 0)) < 0) {
      ec = nano_to_Error(bytes);
    }
    nn_freemsg(l_buffer);
  } while (running);
}
//-------------------------------------------------------------------------------
//!  URI based constructor. Takes a copy of a URI to setup networking information
//! \param uri [IN]  Service configuration of the new Surveyor
ReqRep_Server::ReqRep_Server(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-------------------------------------------------------------------------------
//!  Shuts down async threading and fress all memory
ReqRep_Server::~ReqRep_Server()
{
  if (_impl->socket) {
    nn_close(_impl->socket);
  }
  _impl->socket = 0;
}
//-------------------------------------------------------------------------------
//!
//! \param func [IN] -- Function that will be called when a message is received by a client
//!
//! Blocking call for receiving a single message
void ReqRep_Server::listen(ListenFunc func)
{
  _impl->message_process_function = func;
  _impl->running = false;
  _impl->listen();
}
//-------------------------------------------------------------------------------
//! \param func [IN] -- Function that will be called when a message is received
//! Non Blocking listen request; Will continue to process inbound messages by calling the ListenFunc
//! Until running is set to false.
//! 
void ReqRep_Server::async_listen(ListenFunc func)
{
  _impl->message_process_function = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-------------------------------------------------------------------------------
//! Part of the Pattern interface stands up all IO not handled by the RIIA
//
void ReqRep_Server::standup()
{
}
//-------------------------------------------------------------------------------
//! Terminates all pending IO will invalidate future broadcast events and should onyl be called
//! Before the Surveyor goes out of scope.
void ReqRep_Server::shutdown()
{
  _impl->running = false;
}
//-------------------------------------------------------------------------------
}
