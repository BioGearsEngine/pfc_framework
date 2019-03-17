#include <iostream>
#include <thread>

#include "Registry.h"
#include <boost/system/error_code.hpp>

int main(int argc, const char* argv[])
{
  std::unique_ptr<pfc::Registry> reg;
  try {
    reg = std::make_unique<pfc::Registry>("0::0", "ff31::8000:1234");

  } catch (boost::system::error_code& e) {
    std::cout << e << std::endl;
  } catch (...) {
    std::cout << "Unknown Exception Caught\n";
  }

  if (reg) {
    reg->start();
  }

  std::cout << "Sleeping for 15 seconds\n";
  std::this_thread::sleep_for(std::chrono::seconds(15));
  std::cout << "Done Sleeping\n";

  if (reg) {
    reg->shutdown();
  }
}
