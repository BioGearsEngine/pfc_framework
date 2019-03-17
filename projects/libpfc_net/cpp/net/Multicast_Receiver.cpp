#include <sustain/framework/net/Multicast_Receiver.h>

#include <thread>

#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>

namespace pfc {

template <typename CharT, typename TraitsT = std::char_traits<CharT>>
class vector_streambuf : public std::basic_streambuf<CharT, TraitsT> {
public:
  vector_streambuf(std::vector<CharT>& vec)
  {
    setg(vec.data(), vec.data(), vec.data() + vec.size());
  }
};

struct Multicast_Receiver::Implementation {
  Implementation();
  ~Implementation();
  Error multicast_setup(const std::string& bind_address, const std::string& multicast_addres, uint16_t ports);
  void multicast_receive();

  boost::asio::io_context io_context;
  boost::asio::ip::udp::socket socket;
  boost::asio::ip::udp::endpoint endpoint;
  std::vector<char> buffer;

  std::thread multicast_async_receive_thread;
  std::function<void(std::istream&)> process_message_function;

  Error system_status;
};
//-----------------------------------------------------------------------------
Multicast_Receiver::Implementation::Implementation()
  : socket(io_context)
{
}
//-----------------------------------------------------------------------------
Multicast_Receiver::Implementation::~Implementation()
{
  if (!io_context.stopped()) {
    io_context.stop();
  }
  if (multicast_async_receive_thread.joinable()) {
    multicast_async_receive_thread.join();
  }
}
//-----------------------------------------------------------------------------
Error Multicast_Receiver::Implementation::multicast_setup(const std::string& bind_address, const std::string& multicast_address, uint16_t port)
{
  boost::system::error_code ec;
  auto listen_address = boost::asio::ip::make_address(bind_address, ec);
  if (!ec) {
    boost::asio::ip::udp::endpoint listen_endpoint(
      listen_address, port);
    socket.open(listen_endpoint.protocol());
    socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket.bind(listen_endpoint);
    // Join the multicast group.
    auto multicast_endpoint = boost::asio::ip::make_address(multicast_address, ec);
    if (!ec) {
      socket.set_option(
        boost::asio::ip::multicast::join_group(multicast_endpoint));
    } else {
      system_status = Error::Code::PFC_IP_PARSE_ERROR;
    }
  } else {
    system_status = Error::Code::PFC_IP_PARSE_ERROR;
  }
  return system_status;
}
//-----------------------------------------------------------------------------
void Multicast_Receiver::Implementation::multicast_receive()
{
  buffer.resize(1024);
  socket.async_receive_from(
    boost::asio::buffer(buffer), endpoint,
    [this](boost::system::error_code ec, std::size_t length) {
      if (!ec) {
        vector_streambuf<char> in_buffer{ buffer };
        std::istream stream{ &in_buffer };
        process_message_function(stream);
        multicast_receive();
      }
    });
}
//-----------------------------------------------------------------------------
Multicast_Receiver::Multicast_Receiver(std::string bind_address, std::string multicast_address, uint16_t port)
  : _impl(std::make_unique<Implementation>())
{
  _impl->multicast_setup(bind_address, multicast_address, port);
}
//-----------------------------------------------------------------------------
Multicast_Receiver::Multicast_Receiver(Multicast_Receiver&& obj)
  : _impl(std::move(obj._impl))
{
}
//-----------------------------------------------------------------------------
Multicast_Receiver::~Multicast_Receiver()
{
  stop();
  join();
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
//! \param process_message_function [IN] std::function<void( const std::ostream& )> - Function to be excuted everytime a message is received.
//!
//! This funciton blocks until it has received one message
void Multicast_Receiver::receive(std::function<void(std::istream&)> process_message_function)
{
  _impl->process_message_function = process_message_function;
  _impl->multicast_receive();
  _impl->io_context.run_one();
}
//-----------------------------------------------------------------------------
//! \param process_message_function [IN] std::function<void( const std::ostream& )> - Function to be excuted everytime a message is received.
//!
//! This funciton receives in a background thread call join to verify the message was received
void Multicast_Receiver::async_receive(std::function<void(std::istream&)> process_message_function)
{
  _impl->process_message_function = process_message_function;
  _impl->multicast_receive();
  _impl->multicast_async_receive_thread = std::thread([this]() { _impl->io_context.run(); });
}
//-----------------------------------------------------------------------------
//! This function waits until no pending work is done.
void Multicast_Receiver::join()
{
  if (_impl->multicast_async_receive_thread.joinable())
    _impl->multicast_async_receive_thread.join();
}
//-----------------------------------------------------------------------------
//! This function waits until no pending work is done.
void Multicast_Receiver::stop()
{
  if (!_impl->io_context.stopped()) {
    _impl->io_context.stop();
  }
}
//-----------------------------------------------------------------------------
size_t Multicast_Receiver::buffer_legth() const { return _impl->buffer.size(); }
//-----------------------------------------------------------------------------
void Multicast_Receiver::buffer_legth(const size_t length) { return _impl->buffer.resize(length); }
//-----------------------------------------------------------------------------
bool Multicast_Receiver::is_valid()
{
  return _impl->system_status == Success();
}
//-----------------------------------------------------------------------------
Error Multicast_Receiver::error()
{
  return _impl->system_status;
}
//-----------------------------------------------------------------------------
Multicast_Receiver& Multicast_Receiver::operator=(Multicast_Receiver&& obj)
{
  _impl = std::move(obj._impl);
  obj._impl = std::make_unique<Implementation>();
  return *this;
}
}
