#include <sustain/framework/net/patterns/pub_sub/Publisher.h>

#include <iostream>
#include <thread>

#include <nanomsg/pubsub.h>

#include "../nanomsg_helper.h"

namespace pfc {

struct nn_buffer : std::streambuf {
  nn_buffer(char* begin, char* end)
  {
    this->setg(begin, begin, end);
  }
};

struct PubSub_Publisher::Implementation {
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

  void publish();

  std::thread pubsub_main_thread;
  BroadcastFunc generate_message_func;

  Error ec;
};
//-------------------------------------------------------------------------------
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
PubSub_Publisher::PubSub_Publisher(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-----------------------------------------------------------------------------
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
void PubSub_Publisher::broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = false;
  _impl->publish();
}
//-----------------------------------------------------------------------------
void PubSub_Publisher::async_broadcast(BroadcastFunc func)
{
  _impl->generate_message_func = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::publish, _impl.get());
}
//-----------------------------------------------------------------------------
void PubSub_Publisher::standup()
{
}
//-----------------------------------------------------------------------------
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