#include "units/GUnitTest.hpp"
#include "areg/base/NESocket.hpp"
#include <gmock/gmock.h>

#include <memory>


namespace
{
class MockSocket
{
public:
    MOCK_METHOD(bool, _osInitSocket, ());
};

std::unique_ptr<MockSocket> mockSocket {nullptr};
}

namespace NESocketWrapper
{
bool _osInitSocket()
{
    if (mockSocket)
    {
        return mockSocket->_osInitSocket();
    }
    return false;
}
}

class NESocketTest : public testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;
};

void NESocketTest::SetUp()
{
    mockSocket = std::make_unique<MockSocket>();
}

void NESocketTest::TearDown()
{
    mockSocket.reset();
}


TEST_F(NESocketTest, testInitSocket)
{
    EXPECT_CALL(*mockSocket, _osInitSocket).WillOnce(testing::Return(true));
    NESocket::socketInitialize();
}

