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

#include <iostream>
#include <thread>

#include "Registry.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>

int main(int argc, const char* argv[])
{
  namespace bpo = boost::program_options;
  bpo::options_description options("Allowed options");
  options.add_options()("help,h", "Produce help message") //
    ("bind,b", bpo::value<std::string>()->default_value("0::0"), "Server port bind address") //
    ("multicast,m", bpo::value<std::string>()->default_value("ff31::8000:1234"), "Server multicast broadcast address");

  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, options), vm);
  bpo::notify(vm);

  if (vm.count("help")) {
    std::cout << options << "\n";
    return 1;
}

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
  } else {
    BOOST_LOG_TRIVIAL(error) << "PFC Registry failed to start\n";
  }
  context.run();
}
