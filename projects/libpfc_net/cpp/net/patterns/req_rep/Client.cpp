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

#include <sustain/framework/net/patterns/req_rep/Client.h>

#include <thread>

#include <nanomsg/reqrep.h>

#include "../nanomsg_helper.h"

namespace pfc {

struct nn_buffer : std::streambuf {
  nn_buffer(char* begin, char* end)
  {
    this->setg(begin, begin, end);
  }
};

struct ReqRep_Client::Implementation {
  Implementation(URI&&);
  ~Implementation();

  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  Implementation& operator==(const Implementation&) = delete;
  Implementation& operator==(Implementation&&) = delete;

  URI uri;
  int socket;
  int rv;
  char* msg_buffer;
  bool running;

  void listen();

  std::thread pubsub_main_thread;
  BroadcastFunc generate_message_func;
  CallbackFunc response_callback_function;

  Error ec;
};
  //-------------------------------------------------------------------------------
ReqRep_Client::Implementation::~Implementation()
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
ReqRep_Client::Implementation::Implementation(URI&& u)
  : uri(std::move(u))
  , socket(0)
  , rv(0)
  , msg_buffer(nullptr)
  , running(false)
{
  if ((socket = nn_socket(AF_SP, NN_REQ)) < 0) {
    ec = nano_to_Error(socket);
  }
  if (rv = nn_connect(socket, uri.c_str()) < 0) {
    ec = nano_to_Error(socket);
  }
}
//-----------------------------------------------------------------------------
void ReqRep_Client::Implementation::listen()
{
  do {
    int bytes = 0;
    auto buffer = generate_message_func();
    if ((bytes = nn_send(socket, &buffer[0], buffer.size(), 0)) < 0) {
      ec = nano_to_Error(bytes);
    }

    char* nanomsg_buffer = 0;
    if ((bytes = nn_recv(socket, &nanomsg_buffer, NN_MSG, 0)) < 0) {
      ec = nano_to_Error(bytes);
    }
    {
      nn_buffer result_buffer(nanomsg_buffer, nanomsg_buffer + bytes);
      std::istream result_stream(&result_buffer);
      if (response_callback_function) {
        response_callback_function(result_stream);
      }
    }
    nn_freemsg(nanomsg_buffer);
  } while (running);
}
//-----------------------------------------------------------------------------
ReqRep_Client::ReqRep_Client(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-----------------------------------------------------------------------------
ReqRep_Client::~ReqRep_Client()
{
  nn_close(_impl->socket);
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
void ReqRep_Client::set_response_callaback_func(CallbackFunc func)
{
  _impl->response_callback_function = func;
}
//-----------------------------------------------------------------------------
void ReqRep_Client::broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = false;
  _impl->listen();
}
//-----------------------------------------------------------------------------
void ReqRep_Client::async_broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-----------------------------------------------------------------------------
void ReqRep_Client::standup()
{
}
//-----------------------------------------------------------------------------
void ReqRep_Client::shutdown()
{
  _impl->running = true;
  if (_impl->socket) {
    nn_close(_impl->socket);
    _impl->socket = 0;
  }
}
//-----------------------------------------------------------------------------
}
