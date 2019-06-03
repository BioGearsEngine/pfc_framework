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

#include <sustain/framework/net/patterns/survey/Surveyor.h>

#include <iostream>
#include <thread>

#include "../nanomsg_helper.h"
#include <boost/log/trivial.hpp>
#include <nanomsg/survey.h>

namespace pfc {

struct nn_buffer : std::streambuf {
  nn_buffer(char* begin, char* end)
  {
    this->setg(begin, begin, end);
  }
};

struct Survey_Surveyor::Implementation {
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
Survey_Surveyor::Implementation::Implementation(URI&& u)
  : uri(std::move(u))
  , socket(0)
  , rv(0)
  , msg_buffer(nullptr)
  , running(false)
{
  if ((socket = nn_socket(AF_SP, NN_SURVEYOR)) < 0) {
    ec = nano_to_Error(socket);
  }
  if (nn_bind(socket, uri.c_str()) < 0) {
    ec = nano_to_Error(socket);
  }
}
//-------------------------------------------------------------------------------
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
Survey_Surveyor::Survey_Surveyor(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-----------------------------------------------------------------------------
Survey_Surveyor::~Survey_Surveyor()
{
  nn_close(_impl->socket);
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
void Survey_Surveyor::set_response_callaback_func(CallbackFunc func)
{
  _impl->response_callback_function = func;
}
//-----------------------------------------------------------------------------
void Survey_Surveyor::broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = false;
  _impl->listen();
}
//-----------------------------------------------------------------------------
void Survey_Surveyor::async_broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-----------------------------------------------------------------------------
void Survey_Surveyor::standup()
{
}
//-----------------------------------------------------------------------------
void Survey_Surveyor::shutdown()
{
  _impl->running = true;
  if (_impl->socket) {
    nn_close(_impl->socket);
    _impl->socket = 0;
  }
}
//-----------------------------------------------------------------------------
}
