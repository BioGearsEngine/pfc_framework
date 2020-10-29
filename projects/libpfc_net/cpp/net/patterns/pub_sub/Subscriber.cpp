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

#include <sustain/framework/net/patterns/pub_sub/Subscriber.h>

#include <thread>

#include <nanomsg/pubsub.h>

#include "../nanomsg_helper.h"
#include <sustain/framework/util/Error.h>

namespace pfc {
//!
//! PIMPL Implementation of a PubSub_Subscriber
struct PubSub_Subscriber::Implementation {
  Implementation(URI&&);
  ~Implementation();

  URI uri; //!< Boost io_context used in this Multicast_Sender
  int socket; //!< Multicast broadcast channel
  int rv; //!< Socket used to send messages
  char* msg_buffer; //!< timeout clock
  bool running; //!< Message buffer that broadcast will be stored in

  void listen();

  std::thread pubsub_main_thread; //!< Thread control for async braodcast request. 
  ListenFunc message_process_function; //!<  Callback for processing received broadcast

  Error ec; //!< Current Error code of the system else Success()
};
//-------------------------------------------------------------------------------
//! Constructs an Implementation of a PubSub_Subscriber
PubSub_Subscriber::Implementation::~Implementation()
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
//!  URI based constructor. Takes a copy of a URI to setup networking information
//! \param u [IN]  Service configuration of the new Surveyor
PubSub_Subscriber::Implementation::Implementation(URI&& u)
  : uri(std::move(u))
  , socket(0)
  , rv(0)
  , msg_buffer(nullptr)
  , running(false)
{
  if ((socket = nn_socket(AF_SP, NN_SUB)) < 0) {
    ec = nano_to_Error(socket);
  }
  if (nn_setsockopt(socket, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) < 0) {
    ec = nano_to_Error(socket);
  }
  if (nn_connect(socket, uri.c_str()) < 0) {
    ec = nano_to_Error(socket);
  }
}
//-------------------------------------------------------------------------------
//!
//! Listen function for controlling PubSub paradigm.
//! Underlying Implementation is done in nanomsg
//!
void PubSub_Subscriber::Implementation::listen()
{
  do {
    char* l_buffer = NULL;
    int bytes;
    if ((bytes = nn_recv(socket, &l_buffer, NN_MSG, 0)) < 0) {
      ec = nano_to_Error(bytes);
    }
    std::vector<char> response = message_process_function(l_buffer, bytes);
    nn_freemsg(l_buffer);
  } while (running);
}
//-------------------------------------------------------------------------------
//!  URI based constructor. Takes a copy of a URI to setup networking information
//! \param uri [IN]  Service configuration of the new PubSub_Subscriber
PubSub_Subscriber::PubSub_Subscriber(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-------------------------------------------------------------------------------
//!  Shuts down async threading and fress all memory
PubSub_Subscriber::~PubSub_Subscriber()
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
void PubSub_Subscriber::listen(ListenFunc func)
{
  _impl->message_process_function = func;
  _impl->running = false;
  _impl->listen();
}
//-------------------------------------------------------------------------------
//! \param func [IN] -- Function that will be called when a message is received
//! Non Blocking listen request; Will continue to process inbound messages by calling the ListenFunc
//! Until running is set to false.
//! Future versions of async_broadcast may allow multiple parallel broadcast, but this is currently
//! Undefined behavior.
//!
void PubSub_Subscriber::async_listen(ListenFunc func)
{
  _impl->message_process_function = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-------------------------------------------------------------------------------
//! Part of the Pattern interface stands up all IO not handled by the RIIA
//
void PubSub_Subscriber::standup()
{
}
//-------------------------------------------------------------------------------
//! Terminates all pending IO will invalidate future broadcast events and should onyl be called
//! Before the Surveyor goes out of scope.
void PubSub_Subscriber::shutdown()
{
  _impl->running = false;
}
//-------------------------------------------------------------------------------
}
