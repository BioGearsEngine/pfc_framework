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

#include <sustain/framework/net/Multicast_Receiver.h>

#include <thread>

#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>

namespace pfc {

//!
//!   basic_streambuf wrapper of a vector class.
//!   The vector can be moved in to teh vector_streambuf
//!   To simplify life time issues or copied
//!
template <typename CharT, typename TraitsT = std::char_traits<CharT>>
class vector_streambuf : public std::basic_streambuf<CharT, TraitsT> {
public:
  //!
  //! Moves Vector to self
  //!
  vector_streambuf(std::vector<CharT>&& vec)
    : self(std::move(vec))
  {
    setg(self.data(), self.data(), self.data() + self.size());
  }
  //!
  //! Copies Vector to self
  //!
  vector_streambuf(std::vector<CharT> vec)
    : self(std::move(vec))
  {
    setg(self.data(), self.data(), self.data() + self.size());
  }
  //!
  //! Internally stores the current &vec[0] and vec->size()
  //! It is critical that the vector remain in scope while the vector_streambuf exist
  //! And  any operation which would invlalidate an interator is unsafe for the lifetime of the vector_streambuf
  //!
  vector_streambuf(std::vector<CharT>* vec)
    : self()
  {
    setg(vec->data(), vec->data(), vec->data() + vec->size());
  }

private:
  std::vector<CharT> self; //!< Internal vector storage for move and copy constructors
};

/**
 *  PIMPL Implementation of the Multicast_Receiver
 */
struct Multicast_Receiver::Implementation {
  Implementation();
  ~Implementation();
  Error multicast_setup(const std::string& bind_address, const std::string& multicast_addres, uint16_t ports);
  void multicast_receive();

  boost::asio::io_context io_context; //!< boost::asio IO_CONTEXT
  boost::asio::ip::udp::socket socket; //!< boost::socket for udp broadcast
  boost::asio::ip::udp::endpoint endpoint; //!< boost broadcast endpoint
  std::vector<char> buffer; //!< Internal buffer for receiving messages

  std::thread multicast_async_receive_thread; //!< Thread used for async receceives
  std::function<void(std::istream&)> process_message_function; //!<Callback function for processing messages once received. Will be passed in by derived classes

  Error system_status; //!< Current System Status
};
//-----------------------------------------------------------------------------
//!
//! Default constructor for an Implementation
//!
Multicast_Receiver::Implementation::Implementation()
  : socket(io_context)
{
}
//-----------------------------------------------------------------------------
//!
//! Stops current threading activity and destroys the implementation
//!
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
//!
//! Setup function for multicast configuration
//! \param bind_address [IN] - Interface Bind Address for the multicast device
//! \param multicast_address [IN] - UDP Braodcast channel to subscribe to
//! \param port [IN] - Port for the broadcast channel to listen on
//!
//! \return Error - PFC_IP_PARSE_ERROR if paramaters are not ithe proper format else NO_ERROR
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
//!
//! Blocking call to  listen on multicast channel
//!
void Multicast_Receiver::Implementation::multicast_receive()
{
  if (buffer.size() < 1024) {
    buffer.resize(1024);
  }
  socket.async_receive_from(
    boost::asio::buffer(buffer), endpoint,
    [this](boost::system::error_code ec, std::size_t length) {
      if (!ec) {
        vector_streambuf<char> in_buffer { &buffer };
        std::istream stream { &in_buffer };
        process_message_function(stream);
        multicast_receive();
      }
    });
}
//-----------------------------------------------------------------------------
//!
//! \param bind_address [IN] - Interface Bind Address for the multicast device
//! \param multicast_address [IN] - UDP Braodcast channel to subscribe to
//! \param port [IN] - Port for the broadcast channel to listen on
//!
Multicast_Receiver::Multicast_Receiver(std::string bind_address, std::string multicast_address, uint16_t port)
  : _impl(std::make_unique<Implementation>())
{
  _impl->multicast_setup(bind_address, multicast_address, port);
}
//-----------------------------------------------------------------------------
//! Copy constructof ro Multicast_Receiver
//! \param obj [IN,OUT] - Object to be moved in to this
Multicast_Receiver::Multicast_Receiver(Multicast_Receiver&& obj)
  : _impl(std::move(obj._impl))
{
}
//-----------------------------------------------------------------------------
//! Deconstructor of Multicast_Receiver will terminate all pending IO
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
//! \return size_t -- Lengh of the current underlying buffer
size_t Multicast_Receiver::buffer_legth() const { return _impl->buffer.size(); }
//-----------------------------------------------------------------------------
//! \return size_t [IN] -- Resizes the background buffer. Implementation will inflate to a minimum of 1024 bytes but will respect larger sizes.
void Multicast_Receiver::buffer_legth(const size_t length) { return _impl->buffer.resize(length); }
//-----------------------------------------------------------------------------
//! \return bool -- True if System_Status is Success();
bool Multicast_Receiver::is_valid()
{
  return _impl->system_status == Success();
}
//-----------------------------------------------------------------------------
//! \return Error -- Returns Success() or the last ErrorCode set
Error Multicast_Receiver::error()
{
  return _impl->system_status;
}
//-----------------------------------------------------------------------------
//! \return bool -- Move operator for a Multicast_Receiver
Multicast_Receiver& Multicast_Receiver::operator=(Multicast_Receiver&& obj)
{
  _impl = std::move(obj._impl);
  obj._impl = std::make_unique<Implementation>();
  return *this;
}
}
