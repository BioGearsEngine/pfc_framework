#include <sustain/framework/pub_sub/Subscriber.h>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <sustain/framework/util/Error.h>

namespace pfc {
struct Subscriber::Implementation {
  Implementation();
  Implementation(const std::string& service_name, const std::string& multicast_address);
  ~Implementation();
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = default;
  Implementation& operator=(const Implementation&) = delete;
  Implementation& operator=(Implementation&&) = delete;

  pfc::Error multicast_setup(const std::string& multicast_address);
  void multicast_announce();
  void multicast_announce_timeout();

  std::thread multicast_announce_thread;
  std::thread multicast_listen_thread;

  std::atomic<bool> thead_control_shutdown;
  std::atomic<bool> thead_control_service_registered;

  boost::asio::io_context io_context;
  boost::asio::ip::udp::endpoint endpoint;
  boost::asio::ip::udp::socket socket;
  boost::asio::steady_timer timer;

  static constexpr int max_message_count = 25;
  int message_count;
  std::string message;

  //TODO: Determine Optimum Size
  std::array<char, 4096> data;

  pfc::Error server_status;
};
//-----------------------------------------------------------------------------
Subscriber::Implementation::Implementation()
  : io_context()
  , socket(io_context)
  , timer(io_context)
{
}
//-----------------------------------------------------------------------------
Subscriber::Implementation::Implementation(const std::string& service_name, const std::string& multicast_address)
  : io_context()
  , socket(io_context)
  , endpoint()
  , timer(io_context)
  , thead_control_shutdown(false)
  , thead_control_service_registered(false)
  , message_count(0)
  , message("Tutorial Message")
  , server_status(Success())
{
  server_status |= multicast_setup(multicast_address);
}
//-----------------------------------------------------------------------------
Subscriber::Implementation::~Implementation()
{
  thead_control_shutdown = true;
  if (multicast_announce_thread.joinable()) {
    multicast_announce_thread = std::thread([]() {});
  }
  if (multicast_listen_thread.joinable()) {
    multicast_announce_thread = std::thread([]() {});
  }
}
//-----------------------------------------------------------------------------
pfc::Error Subscriber::Implementation::multicast_setup(const std::string& multicast_address)
{
  boost::system::error_code err;
  pfc::Error pfc_error_code = Success();
  auto broadcast_address = boost::asio::ip::make_address(multicast_address, err);
  if (!err) {
    endpoint = boost::asio::ip::udp::endpoint(broadcast_address, g_pfc_registry_announce_port);
    socket = boost::asio::ip::udp::socket(io_context, endpoint.protocol());
  } else {
    pfc_error_code = pfc::Error(Error::Code::PFC_IP_PARSE_ERROR);
  }
  return pfc_error_code;
}
//-----------------------------------------------------------------------------
void Subscriber::Implementation::multicast_announce()
{
  std::ostringstream os;
  os << "Message " << message_count++;
  message = os.str();

  socket.async_send_to(
    boost::asio::buffer(message), endpoint,
    [this](boost::system::error_code ec, std::size_t /*length*/) {
      if (!ec && message_count < max_message_count)
        multicast_announce_timeout();
    });
}
//-----------------------------------------------------------------------------
void Subscriber::Implementation::multicast_announce_timeout()
{
  timer.expires_after(std::chrono::seconds(1));
  timer.async_wait(
    [this](boost::system::error_code ec) {
      if (!ec)
        multicast_announce();
    });
}
//-----------------------------------------------------------------------------
Subscriber::Subscriber(std::string service_name, std::string multicast_address)
  : _impl(std::make_unique<Implementation>(std::move(service_name), std::move(multicast_address)))
{
}
//-----------------------------------------------------------------------------
Subscriber::Subscriber(Subscriber&& obj)
  : _impl(std::move(obj._impl))
{
  obj._impl = std::make_unique<Implementation>();
}
//-----------------------------------------------------------------------------
Subscriber::~Subscriber()
{

}
//-----------------------------------------------------------------------------
bool Subscriber::Valid()
{
  return _impl->server_status.is_ok();
}
//-----------------------------------------------------------------------------
Error Subscriber::Error()
{
  return _impl->server_status;
}
//-----------------------------------------------------------------------------
Subscriber& Subscriber::operator=(Subscriber&& obj)
{
  _impl = std::move(obj._impl);
  obj._impl = std::make_unique<Implementation>();
  return *this;
}
}