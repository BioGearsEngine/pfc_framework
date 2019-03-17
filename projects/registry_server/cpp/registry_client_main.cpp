#include <iostream>
#include <thread>



#include <sustain/framework/Registry.h>
#include <sustain/framework/Service.h>
#include <boost/system/error_code.hpp>

int main(int argc, const char* argv[])
{
  std::unique_ptr<pfc::Registry> reg;
  std::unique_ptr<pfc::Service> service;
  try
  {
    reg = std::make_unique<pfc::Registry>("0::0", "ff31::8000:1234");
    const pfc::Service::Config service_config {
         pfc::Service::Config::pub_sub
         ,3000
         ,"Demo Config"
         ,"192.168.1.1"
         ,"A Demonstration of Registration"
      };
    service = std::make_unique<pfc::Service>(service_config, "0::0", "ff31::8000:1234");
    
  } catch (boost::system::error_code& e)
  {
    std::cout << e << std::endl;
  } catch (...)
  {
    std::cout << "Unknown Exception Caught\n";
  }

  if(reg && service)
  {
    reg->start();
    service->start();
  }

  std::cout << "Sleeping for 15 seconds\n";
  std::this_thread::sleep_for(std::chrono::seconds(15));
  std::cout << "Done Sleeping\n";

    if(reg && service)
  {
    reg->shutdown();
    service->stop();

    service->join();
  }
}
