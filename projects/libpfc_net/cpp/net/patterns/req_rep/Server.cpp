#include <sustain/framework/net/patterns/req_rep/Server.h>

#include <thread>

#include <nanomsg/reqrep.h>

#include "../nanomsg_helper.h"
#include <sustain/framework/util/Error.h>
namespace pfc {
struct ReqRep_Server::Implementation {
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
ReqRep_Server::ReqRep_Server(URI uri)
  : _impl(std::make_unique<Implementation>(std::move(uri)))
{
}
//-------------------------------------------------------------------------------
ReqRep_Server::~ReqRep_Server()
{
  if (_impl->socket) {
    nn_close(_impl->socket);
  }
  _impl->socket = 0;
}
//-------------------------------------------------------------------------------
void ReqRep_Server::listen(ListenFunc func)
{
  _impl->message_process_function = func;
  _impl->running = false;
  _impl->listen();
}
//-------------------------------------------------------------------------------
void ReqRep_Server::async_listen(ListenFunc func)
{
  _impl->message_process_function = func;
  _impl->running = true;
  _impl->pubsub_main_thread = std::thread(&Implementation::listen, _impl.get());
}
//-------------------------------------------------------------------------------
void ReqRep_Server::standup()
{
}
//-------------------------------------------------------------------------------
void ReqRep_Server::shutdown()
{
  _impl->running = false;
}
//-------------------------------------------------------------------------------
}