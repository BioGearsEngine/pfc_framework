#include <iostream>
#include <thread>

#include "Registry.h"

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

int main(int argc, const char* argv[])
{
  namespace bpo = boost::program_options;
  bpo::options_description options("Allowed options");
  options.add_options()("help", "Produce help message") //
    ("bind", bpo::value<std::string>()->default_value("0::0"), "Server port bind address [0::0]") //
    ("multicast", bpo::value<std::string>()->default_value("ff31::8000:1234"), "Server multicast broadcast address [ff31::8000:1234]");

  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, options), vm);
  bpo::notify(vm);

  std::unique_ptr<pfc::Registry> reg;
  try {
    reg = std::make_unique<pfc::Registry>(vm["bind"].as<std::string>(), vm["multicast"].as<std::string>());

  } catch (std::exception& e) {
    std::cerr << e.what();
  }

  boost::asio::io_context context;
  boost::asio::signal_set signals(context);

  signals.add(SIGINT);
  signals.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif
  signals.async_wait([&](const boost::system::error_code& /*ec*/, int /*no*/) {
    BOOST_LOG_TRIVIAL(info) << "Stopping PFC Registry\n";
    reg->shutdown();
  });

  // Start an asynchronous wait for one of the signals to occur.
  if (reg) {
    BOOST_LOG_TRIVIAL(info) << "Starting PFC Registry on " << vm["bind"].as<std::string>() << " broadcasting on " << vm["multicast"].as<std::string>() << "\n";
    reg->start();
  }
  context.run();
}
