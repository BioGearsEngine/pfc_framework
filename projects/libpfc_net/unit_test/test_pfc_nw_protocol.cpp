/**************************************************************************************
Copyright 2019 Applied Research Associates, Inc.
Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License. You may obtain a copy of the License
at:
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
**************************************************************************************/

#include <sustain/framework/Protocol.h>


#include <limits>
#include <thread>

#include <gtest/gtest.h>

#ifdef DISABLE_SUSTAIN_MajorSystems_TEST
#define TEST_FIXTURE_NAME DISABLED_MajorSystems_Fixture
#else
#define TEST_FIXTURE_NAME SEMajorSystems_Fixture
#endif


// The fixture for testing class Foo.
class TEST_FIXTURE_NAME : public ::testing::Test {
protected:
  // You can do set-up work for each test here.
  TEST_FIXTURE_NAME() = default;

  // You can do clean-up work that doesn't throw exceptions here.
  virtual ~TEST_FIXTURE_NAME() = default;

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  // Code here will be called immediately after the constructor (right
  // before each test).
  virtual void SetUp();

  // Code here will be called immediately after each test (right
  // before the destructor).
  virtual void TearDown();

  
};

void TEST_FIXTURE_NAME::SetUp()
{
 
}

void TEST_FIXTURE_NAME::TearDown()
{
  
}

TEST_F(TEST_FIXTURE_NAME, pfc_service_registry_request)
{
  using namespace pfc;

  pfc_service_announcement outbound;
  outbound._protacol = pfc_protocol::pub_sub;
  outbound._port = 0xDEAD;
  outbound._address = "192.168.1.1";
  outbound._name    = "Unit Test Service";
  outbound._brief   = "is this the right thing";
  pfc_service_announcement inbound;

  std::stringstream ss;
  //
  auto error = outbound.serialize(ss);

  EXPECT_EQ( Error::Code::PFC_NONE , error );

  inbound.deserialize(ss);

  EXPECT_EQ(inbound,outbound);
}
