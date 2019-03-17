#include <sustain/framework/net/Multicast_Sender.h>

#include <thread>

#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>

#include <boost/asio/buffered_stream.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/streambuf.hpp>

namespace pfc {

struct Multicast_Sender::Implementation {

  Implementation();
  ~Implementation();
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  Implementation& operator==(const Implementation&) = delete;
  Implementation& operator==(Implementation&&) = delete;

  Error multicast_setup(const std::string& multicast_addres, uint16_t ports);
  void multicast_broadcast();
  void multicast_timeout();

  boost::asio::io_context io_context;
  boost::asio::ip::udp::endpoint endpoint;
  boost::asio::ip::udp::socket socket;
  boost::asio::steady_timer timer;
  boost::asio::streambuf buffer;

  std::thread multicast_async_broadcast_thread;
  std::function<void(std::ostream&)> process_message_function;

  Error system_status;
};
//-----------------------------------------------------------------------------
Multicast_Sender::Implementation::Implementation()
  : io_context()
  , endpoint()
  , socket(io_context)
  , timer(io_context)
{
}
//-----------------------------------------------------------------------------
Multicast_Sender::Implementation::~Implementation()
{
  if (!io_context.stopped()) {
    io_context.stop();
  }
  if (multicast_async_broadcast_thread.joinable()) {
    multicast_async_broadcast_thread.join();
  }
}
//-----------------------------------------------------------------------------
Error Multicast_Sender::Implementation::multicast_setup(const std::string& multicast_addres, uint16_t ports)
{
  boost::system::error_code ec;
  const auto multicast_address = boost::asio::ip::make_address(multicast_addres, ec);
  if (!ec) {
    endpoint = boost::asio::ip::udp::endpoint(multicast_address, ports);
    socket = boost::asio::ip::udp::socket(io_context, endpoint.protocol());
  } else {
    system_status = Error::Code::PFC_IP_PARSE_ERROR;
  }
  return system_status;
}
//-----------------------------------------------------------------------------
void Multicast_Sender::Implementation::multicast_broadcast()
{
  std::ostream os(&buffer);
  process_message_function(os);

  socket.async_send_to(
    buffer.data(), endpoint,
    [this](boost::system::error_code ec, std::size_t /*length*/) {
      if (!ec) {
        multicast_timeout();
      }
    });
}
//-----------------------------------------------------------------------------
void Multicast_Sender::Implementation::multicast_timeout()
{
  timer.expires_after(std::chrono::seconds(1));
  timer.async_wait(
    [this](boost::system::error_code ec) {
      if (!ec)
        multicast_broadcast();
    });
}
//-----------------------------------------------------------------------------
Multicast_Sender::Multicast_Sender(std::string multicast_address, uint16_t port)
  : _impl(std::make_unique<Implementation>())
{
  _impl->multicast_setup(multicast_address, port);
}
//-----------------------------------------------------------------------------
Multicast_Sender::Multicast_Sender(Multicast_Sender&& obj)
  : _impl(std::move(obj._impl))
{
}
//-----------------------------------------------------------------------------
Multicast_Sender::~Multicast_Sender()
{
  stop();
  join();
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
void Multicast_Sender::send(std::function<void(std::ostream&)> process_message_function)
{
  _impl->process_message_function = process_message_function;
  _impl->multicast_broadcast();
  _impl->io_context.run_one();
}
//-----------------------------------------------------------------------------
void Multicast_Sender::async_send(std::function<void(std::ostream&)> process_message_function)
{
  _impl->process_message_function = process_message_function;
  _impl->multicast_broadcast();
  _impl->multicast_async_broadcast_thread = std::thread([this]() { _impl->io_context.run(); });
}
//-----------------------------------------------------------------------------
void Multicast_Sender::join()
{
  if (_impl->multicast_async_broadcast_thread.joinable()) {
    _impl->multicast_async_broadcast_thread.join();
  }
}
//-----------------------------------------------------------------------------
void Multicast_Sender::stop()
{
  if (!_impl->io_context.stopped()) {
    _impl->io_context.stop();
  }
}
//-----------------------------------------------------------------------------
bool Multicast_Sender::is_valid()
{
  return _impl->system_status == Success();
}
//-----------------------------------------------------------------------------
Error Multicast_Sender::error() const
{
  return _impl->system_status;
}
//-----------------------------------------------------------------------------
Multicast_Sender& Multicast_Sender::operator=(Multicast_Sender&& rhs)
{
  _impl = std::move(rhs._impl);
  return *this;
}
}
