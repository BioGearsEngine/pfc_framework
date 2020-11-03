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
//!
//!  PIMPL Implementation of a Multicast Sender
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

  boost::asio::io_context io_context;        //!< Boost io_context used in this Multicast_Sender
  boost::asio::ip::udp::endpoint endpoint;   //!< Multicast broadcast channel
  boost::asio::ip::udp::socket socket;       //!< Socket used to send messages
  boost::asio::steady_timer timer;           //!< timeout clock
  boost::asio::streambuf buffer;             //!< Message buffer that broadcast will be stored in

  std::thread multicast_async_broadcast_thread; //!< Thread control for async braodcast request. 
  std::function<void(std::ostream&)> process_message_function;    //!<  Callback for processing received broadcast

  Error system_status; //!< Current Error code of the system else Success()
};
//-----------------------------------------------------------------------------
//! Constructs an Implementation of a Multicast_Sender
Multicast_Sender::Implementation::Implementation()
  : io_context()
  , endpoint()
  , socket(io_context)
  , timer(io_context)
{
}
//-----------------------------------------------------------------------------
//! Deconstructs a Multicast_Sender and shuts down pending IO
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
//! \param multicast_address [IN] broadcast channel to be used for multicast
//! \param port [IN] port on the address ot be used
//! \return Error - Success() unless an PFC_IP_PARSE_ERROR occurs 
Error Multicast_Sender::Implementation::multicast_setup(const std::string& multicast_address, uint16_t port)
{
  boost::system::error_code ec;
  const auto boost_multicast_address = boost::asio::ip::make_address(multicast_address, ec);
  if (!ec) {
    endpoint = boost::asio::ip::udp::endpoint(boost_multicast_address, port);
    socket = boost::asio::ip::udp::socket(io_context, endpoint.protocol());
  } else {
    system_status = Error::Code::PFC_IP_PARSE_ERROR;
  }
  return system_status;
}
//-----------------------------------------------------------------------------
//! Broadcast a single multicast message using blocking IO
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
//! Handles multicast timeouts by reseting broadcast message after a short delay
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
//! \param multicast_address [IN] broadcast channel to be used for multicast
//! \param port [IN] port on the address ot be used
  : _impl(std::make_unique<Implementation>())
{
  _impl->multicast_setup(multicast_address, port);
}
//-----------------------------------------------------------------------------
//! Default copy constructor for Multicast_sender
//! \param obj [IN,OUT] obj to be moved in this address
Multicast_Sender::Multicast_Sender(Multicast_Sender&& obj)
  : _impl(std::move(obj._impl))
{
}
//-----------------------------------------------------------------------------
//! Deconstructs Multicast_Sender and stops all async IO
Multicast_Sender::~Multicast_Sender()
{
  stop();
  join();
  _impl = nullptr;
}
//-----------------------------------------------------------------------------
//! \param process_message_function [IN] std::function<void( const std::ostream& )> - Function to be excuted everytime a message is received.
//!
//! Blocking call to send a single multicast message
void Multicast_Sender::send(std::function<void(std::ostream&)> process_message_function)
{
  _impl->process_message_function = process_message_function;
  _impl->multicast_broadcast();
  _impl->io_context.run_one();
}
//-----------------------------------------------------------------------------
//! \param process_message_function [IN] std::function<void( const std::ostream& )> - Function to be excuted everytime a message is received.
//!
//! Sends a Message asyncronisly  stopping once io_context closes
void Multicast_Sender::async_send(std::function<void(std::ostream&)> process_message_function)
{
  _impl->process_message_function = process_message_function;
  _impl->multicast_broadcast();
  _impl->multicast_async_broadcast_thread = std::thread([this]() { _impl->io_context.run(); });
}
//-----------------------------------------------------------------------------
//! Blocking call  until all async IO has been stopped. 
void Multicast_Sender::join()
{
  if (_impl->multicast_async_broadcast_thread.joinable()) {
    _impl->multicast_async_broadcast_thread.join();
  }
}
//-----------------------------------------------------------------------------
//! Stops current async IO
void Multicast_Sender::stop()
{
  if (!_impl->io_context.stopped()) {
    _impl->io_context.stop();
  }
}
//-----------------------------------------------------------------------------
//! \return bool -- true if error() == Success()
bool Multicast_Sender::is_valid()
{
  return _impl->system_status == Success();
}
//-----------------------------------------------------------------------------
//! \return Error -- Success() unless a processing error has occured
Error Multicast_Sender::error() const
{
  return _impl->system_status;
}
//----------------------------------------------------------------------------
//! Move operator for Multicast_Sender
//! \param rhs [IN,OUT] -- RHS to be moved in to this
//! \return Multicast_Sender& -- Reference to this
Multicast_Sender& Multicast_Sender::operator=(Multicast_Sender&& rhs)
{
  _impl = std::move(rhs._impl);
  return *this;
}
}
