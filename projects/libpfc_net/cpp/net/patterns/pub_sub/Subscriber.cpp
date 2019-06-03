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

#include <sustain/framework/net/patterns/pub_sub/Subscriber.h>

#include <thread>

#include <nanomsg/pubsub.h>

#include "../nanomsg_helper.h"
#include <sustain/framework/util/Error.h>

namespace pfc {
struct PubSub_Subscriber::Implementation {
  Implementation(URI&&);
  ~Implementation();

  URI uri;
  int socket;
  int rv;
  char* msg_buffer;
  bool running;

  void listen();

  std::thread pubsub_main_thread;
  ListenFunc message_process_function;

  Error ec;
};
//-------------------------------------------------------------------------------
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
PubSub_Subscriber::PubSub_Subscriber(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-------------------------------------------------------------------------------
PubSub_Subscriber::~PubSub_Subscriber()
{
  if (_impl->socket) {
    nn_close(_impl->socket);
  }
  _impl->socket = 0;
}
//-------------------------------------------------------------------------------
void PubSub_Subscriber::listen(ListenFunc func)
{
  _impl->message_process_function = func;
  _impl->running = false;
  _impl->listen();
}
//-------------------------------------------------------------------------------
void PubSub_Subscriber::async_listen(ListenFunc func)
{
  _impl->message_process_function = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-------------------------------------------------------------------------------
void PubSub_Subscriber::standup()
{
}
//-------------------------------------------------------------------------------
void PubSub_Subscriber::shutdown()
{
  _impl->running = false;
}
//-------------------------------------------------------------------------------
}
