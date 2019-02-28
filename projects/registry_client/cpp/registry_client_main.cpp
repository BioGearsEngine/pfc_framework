#include <iostream>
#include <thread>

#include <sustain/framework/Registry.h>
#include <sustain/framework/Service.h>

int main(int argc, const char* argv[])
{
  pfc::Registry reg("0::0", "ff31::8000:1234");
  reg.start();

  pfc::Service test_service("Test Service", "ff31::8000:1234");
  test_service.start();

  std::this_thread::sleep_for( std::chrono::seconds(20) );
  reg.stop();
  test_service.stop();

  reg.join();
  test_service.join();
}
