/**
 * @file test_sdnetopt.cpp
 * @brief Network options and configuration tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdnetopt.h"
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#endif

using namespace SSCP;

class SDNetOptTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// Test network option constants
TEST_F(SDNetOptTest, ConnectionOptionConstants) {
    // Verify connection option constants are defined correctly
    EXPECT_EQ(CONNECTION_OPT_SOCKOPT, 1u);
    
    // Constants should be compile-time constants
    static_assert(CONNECTION_OPT_SOCKOPT == 1, "CONNECTION_OPT_SOCKOPT should be 1");
}

TEST_F(SDNetOptTest, ListenerOptionConstants) {
    // Verify listener option constants
    EXPECT_EQ(LISTENER_OPT_SESSION_FACTORY, 101u);
    EXPECT_EQ(LISTENER_OPT_WAITMSECFORBUFFER, 102u);
    
    // Constants should be compile-time constants
    static_assert(LISTENER_OPT_SESSION_FACTORY == 101, "LISTENER_OPT_SESSION_FACTORY should be 101");
    static_assert(LISTENER_OPT_WAITMSECFORBUFFER == 102, "LISTENER_OPT_WAITMSECFORBUFFER should be 102");
}

TEST_F(SDNetOptTest, LinuxNetworkOptionConstants) {
    // Verify Linux network option constants
    EXPECT_EQ(NETLIN_OPT_MAX_CONNECTION, 201u);
    EXPECT_EQ(NETLIN_OPT_QUEUE_SIZE, 202u);
    EXPECT_EQ(NETLIN_OPT_ADVANCE_PARAM, 203u);
    
    // Constants should be compile-time constants
    static_assert(NETLIN_OPT_MAX_CONNECTION == 201, "NETLIN_OPT_MAX_CONNECTION should be 201");
    static_assert(NETLIN_OPT_QUEUE_SIZE == 202, "NETLIN_OPT_QUEUE_SIZE should be 202");
    static_assert(NETLIN_OPT_ADVANCE_PARAM == 203, "NETLIN_OPT_ADVANCE_PARAM should be 203");
}

TEST_F(SDNetOptTest, WindowsNetworkOptionConstants) {
    // Verify Windows network option constants
    EXPECT_EQ(NETWIN_OPT_MAX_CONNECTION, 301u);
    EXPECT_EQ(NETWIN_OPT_QUEUE_SIZE, 302u);
    EXPECT_EQ(NETWIN_OPT_ADVANCE_PARAM, 303u);
    EXPECT_EQ(NETWIN_OPT_WORKTHREAD_PARAM, 304u);
    
    // Constants should be compile-time constants
    static_assert(NETWIN_OPT_MAX_CONNECTION == 301, "NETWIN_OPT_MAX_CONNECTION should be 301");
    static_assert(NETWIN_OPT_QUEUE_SIZE == 302, "NETWIN_OPT_QUEUE_SIZE should be 302");
    static_assert(NETWIN_OPT_ADVANCE_PARAM == 303, "NETWIN_OPT_ADVANCE_PARAM should be 303");
    static_assert(NETWIN_OPT_WORKTHREAD_PARAM == 304, "NETWIN_OPT_WORKTHREAD_PARAM should be 304");
}

// Test structure definitions and usage
TEST_F(SDNetOptTest, SConnectionOptSockoptStructure) {
    SConnectionOptSockopt sockOpt;
    
    // Test structure member assignments
    sockOpt.nLevel = 1;
    sockOpt.nOptName = 2;
    sockOpt.pOptVal = "test_value";
    sockOpt.nOptLen = 10;
    
    EXPECT_EQ(sockOpt.nLevel, 1);
    EXPECT_EQ(sockOpt.nOptName, 2);
    EXPECT_STREQ(sockOpt.pOptVal, "test_value");
    EXPECT_EQ(sockOpt.nOptLen, 10);
    
    // Test with null pointer
    sockOpt.pOptVal = nullptr;
    EXPECT_EQ(sockOpt.pOptVal, nullptr);
}

TEST_F(SDNetOptTest, SListenerOptSessionFactoryStructure) {
    SListenerOptSessionFactory sessionFactory;
    
    // Test structure member assignment
    void* testPtr = reinterpret_cast<void*>(0x12345678);
    sessionFactory.pSessionFactory = testPtr;
    
    EXPECT_EQ(sessionFactory.pSessionFactory, testPtr);
    
    // Test with null pointer
    sessionFactory.pSessionFactory = nullptr;
    EXPECT_EQ(sessionFactory.pSessionFactory, nullptr);
}

TEST_F(SDNetOptTest, SNetLinOptMaxConnectionStructure) {
    SNetLinOptMaxConnection maxConn;
    
    // Test structure member assignment
    maxConn.nMaxConnection = 1000;
    EXPECT_EQ(maxConn.nMaxConnection, 1000);
    
    // Test with negative value (indicates use default)
    maxConn.nMaxConnection = -1;
    EXPECT_EQ(maxConn.nMaxConnection, -1);
    
    // Test with zero
    maxConn.nMaxConnection = 0;
    EXPECT_EQ(maxConn.nMaxConnection, 0);
}

TEST_F(SDNetOptTest, SNetLinOptQueueSizeStructure) {
    SNetLinOptQueueSize queueSize;
    
    // Test all structure members
    queueSize.nRecvBufSize = 8192;
    queueSize.nSendBufSize = 4096;
    queueSize.nRequestQueueSize = 1024;
    queueSize.nEventQueueSize = 512;
    
    EXPECT_EQ(queueSize.nRecvBufSize, 8192);
    EXPECT_EQ(queueSize.nSendBufSize, 4096);
    EXPECT_EQ(queueSize.nRequestQueueSize, 1024);
    EXPECT_EQ(queueSize.nEventQueueSize, 512);
    
    // Test with default values (-1)
    queueSize.nRecvBufSize = -1;
    queueSize.nSendBufSize = -1;
    queueSize.nRequestQueueSize = -1;
    queueSize.nEventQueueSize = -1;
    
    EXPECT_EQ(queueSize.nRecvBufSize, -1);
    EXPECT_EQ(queueSize.nSendBufSize, -1);
    EXPECT_EQ(queueSize.nRequestQueueSize, -1);
    EXPECT_EQ(queueSize.nEventQueueSize, -1);
}

TEST_F(SDNetOptTest, SNetLinOptAdvanceParamStructure) {
    SNetLinOptAdvanceParam advParam;
    
    // Test structure members
    advParam.nParam1 = 100;
    advParam.nParam2 = 200;
    
    EXPECT_EQ(advParam.nParam1, 100);
    EXPECT_EQ(advParam.nParam2, 200);
    
    // Test with default values (-1)
    advParam.nParam1 = -1;
    advParam.nParam2 = -1;
    
    EXPECT_EQ(advParam.nParam1, -1);
    EXPECT_EQ(advParam.nParam2, -1);
}

TEST_F(SDNetOptTest, SNetWinOptMaxConnectionStructure) {
    SNetWinOptMaxConnection maxConn;
    
    // Test structure member assignment
    maxConn.nMaxConnection = 2000;
    EXPECT_EQ(maxConn.nMaxConnection, 2000);
    
    // Test with negative value (indicates use default)
    maxConn.nMaxConnection = -1;
    EXPECT_EQ(maxConn.nMaxConnection, -1);
}

TEST_F(SDNetOptTest, SNetWinOptQueueSizeStructure) {
    SNetWinOptQueueSize queueSize;
    
    // Test structure members
    queueSize.nRecvBufSize = 16384;
    queueSize.nEventQueueSize = 1024;
    
    EXPECT_EQ(queueSize.nRecvBufSize, 16384);
    EXPECT_EQ(queueSize.nEventQueueSize, 1024);
    
    // Test with default values (-1)
    queueSize.nRecvBufSize = -1;
    queueSize.nEventQueueSize = -1;
    
    EXPECT_EQ(queueSize.nRecvBufSize, -1);
    EXPECT_EQ(queueSize.nEventQueueSize, -1);
}

TEST_F(SDNetOptTest, SNetWinOptAdvanceParamStructure) {
    SNetWinOptAdvanceParam advParam;
    
    // Test structure member
    advParam.nParam1 = 300;
    EXPECT_EQ(advParam.nParam1, 300);
    
    // Test with default value (-1)
    advParam.nParam1 = -1;
    EXPECT_EQ(advParam.nParam1, -1);
}

TEST_F(SDNetOptTest, SNetWinOptWorkThreadParamStructure) {
    SNetWinOptWorkThreadParam workThreadParam;
    
    // Test structure member
    workThreadParam.nParam1 = 4; // 4 worker threads
    EXPECT_EQ(workThreadParam.nParam1, 4);
    
    // Test with default value (-1)
    workThreadParam.nParam1 = -1;
    EXPECT_EQ(workThreadParam.nParam1, -1);
}

TEST_F(SDNetOptTest, SConnectorOptSockBindStructure) {
    SConnectorOptSockBind sockBind;
    
    // Test structure members
    const char* testIP = "192.168.1.1";
    sockBind.pszIP = testIP;
    sockBind.wPort = 8080;
    
    EXPECT_STREQ(sockBind.pszIP, "192.168.1.1");
    EXPECT_EQ(sockBind.wPort, 8080);
    
    // Test with null IP (won't bind)
    sockBind.pszIP = nullptr;
    sockBind.wPort = 0; // won't bind
    
    EXPECT_EQ(sockBind.pszIP, nullptr);
    EXPECT_EQ(sockBind.wPort, 0);
    
    // Test with localhost
    sockBind.pszIP = "127.0.0.1";
    sockBind.wPort = 9090;
    
    EXPECT_STREQ(sockBind.pszIP, "127.0.0.1");
    EXPECT_EQ(sockBind.wPort, 9090);
}

// Test structure sizes (important for binary compatibility)
TEST_F(SDNetOptTest, StructureSizes) {
    // These tests ensure structure sizes remain consistent
    // Important for binary compatibility and memory layout
    
    EXPECT_GE(sizeof(SConnectionOptSockopt), sizeof(INT32) * 3 + sizeof(const char*));
    EXPECT_GE(sizeof(SListenerOptSessionFactory), sizeof(void*));
    EXPECT_GE(sizeof(SNetLinOptMaxConnection), sizeof(INT32));
    EXPECT_GE(sizeof(SNetLinOptQueueSize), sizeof(INT32) * 4);
    EXPECT_GE(sizeof(SNetLinOptAdvanceParam), sizeof(INT32) * 2);
    EXPECT_GE(sizeof(SNetWinOptMaxConnection), sizeof(INT32));
    EXPECT_GE(sizeof(SNetWinOptQueueSize), sizeof(INT32) * 2);
    EXPECT_GE(sizeof(SNetWinOptAdvanceParam), sizeof(INT32));
    EXPECT_GE(sizeof(SNetWinOptWorkThreadParam), sizeof(INT32));
    EXPECT_GE(sizeof(SConnectorOptSockBind), sizeof(const char*) + sizeof(UINT16));
}

// Test default initialization behavior
TEST_F(SDNetOptTest, DefaultInitialization) {
    // Test that structures can be zero-initialized
    SConnectionOptSockopt sockOpt = {};
    EXPECT_EQ(sockOpt.nLevel, 0);
    EXPECT_EQ(sockOpt.nOptName, 0);
    EXPECT_EQ(sockOpt.pOptVal, nullptr);
    EXPECT_EQ(sockOpt.nOptLen, 0);
    
    SListenerOptSessionFactory sessionFactory = {};
    EXPECT_EQ(sessionFactory.pSessionFactory, nullptr);
    
    SNetLinOptMaxConnection maxConn = {};
    EXPECT_EQ(maxConn.nMaxConnection, 0);
    
    SConnectorOptSockBind sockBind = {};
    EXPECT_EQ(sockBind.pszIP, nullptr);
    EXPECT_EQ(sockBind.wPort, 0);
}

// Test typical usage scenarios
TEST_F(SDNetOptTest, TypicalUsageScenarios) {
    // Scenario 1: Setting up socket options
    SConnectionOptSockopt sockOpt;
    sockOpt.nLevel = SOL_SOCKET;
    sockOpt.nOptName = SO_REUSEADDR;
    int reuseAddr = 1;
    sockOpt.pOptVal = reinterpret_cast<const char*>(&reuseAddr);
    sockOpt.nOptLen = sizeof(reuseAddr);
    
    EXPECT_EQ(sockOpt.nLevel, SOL_SOCKET);
    EXPECT_EQ(sockOpt.nOptName, SO_REUSEADDR);
    EXPECT_EQ(sockOpt.nOptLen, sizeof(int));
    
    // Scenario 2: Configuring Linux network queue sizes
    SNetLinOptQueueSize linuxQueue;
    linuxQueue.nRecvBufSize = 65536;      // 64KB receive buffer
    linuxQueue.nSendBufSize = 32768;      // 32KB send buffer
    linuxQueue.nRequestQueueSize = 1000;  // 1000 requests
    linuxQueue.nEventQueueSize = 500;     // 500 events
    
    EXPECT_EQ(linuxQueue.nRecvBufSize, 65536);
    EXPECT_EQ(linuxQueue.nSendBufSize, 32768);
    EXPECT_EQ(linuxQueue.nRequestQueueSize, 1000);
    EXPECT_EQ(linuxQueue.nEventQueueSize, 500);
    
    // Scenario 3: Configuring Windows network settings
    SNetWinOptQueueSize winQueue;
    winQueue.nRecvBufSize = 131072;       // 128KB receive buffer
    winQueue.nEventQueueSize = 2000;      // 2000 events
    
    EXPECT_EQ(winQueue.nRecvBufSize, 131072);
    EXPECT_EQ(winQueue.nEventQueueSize, 2000);
    
    // Scenario 4: Setting up socket binding
    SConnectorOptSockBind binding;
    binding.pszIP = "0.0.0.0";  // Bind to all interfaces
    binding.wPort = 8080;       // HTTP port
    
    EXPECT_STREQ(binding.pszIP, "0.0.0.0");
    EXPECT_EQ(binding.wPort, 8080);
}

// Test boundary values
TEST_F(SDNetOptTest, BoundaryValues) {
    // Test maximum values
    SNetLinOptMaxConnection maxConn;
    maxConn.nMaxConnection = INT32_MAX;
    EXPECT_EQ(maxConn.nMaxConnection, INT32_MAX);
    
    // Test minimum values
    maxConn.nMaxConnection = INT32_MIN;
    EXPECT_EQ(maxConn.nMaxConnection, INT32_MIN);
    
    // Test port boundaries
    SConnectorOptSockBind sockBind;
    sockBind.wPort = 0;      // Minimum port (won't bind)
    EXPECT_EQ(sockBind.wPort, 0);
    
    sockBind.wPort = 65535;  // Maximum port
    EXPECT_EQ(sockBind.wPort, 65535);
    
    sockBind.wPort = 1024;   // Common minimum for non-privileged ports
    EXPECT_EQ(sockBind.wPort, 1024);
}

// Test array/multiple configuration scenarios
TEST_F(SDNetOptTest, MultipleConfigurations) {
    // Test multiple socket options
    std::vector<SConnectionOptSockopt> socketOptions;
    
    SConnectionOptSockopt opt1;
    opt1.nLevel = SOL_SOCKET;
    opt1.nOptName = SO_REUSEADDR;
    int reuseAddr = 1;
    opt1.pOptVal = reinterpret_cast<const char*>(&reuseAddr);
    opt1.nOptLen = sizeof(reuseAddr);
    socketOptions.push_back(opt1);
    
    SConnectionOptSockopt opt2;
    opt2.nLevel = SOL_SOCKET;
    opt2.nOptName = SO_KEEPALIVE;
    int keepAlive = 1;
    opt2.pOptVal = reinterpret_cast<const char*>(&keepAlive);
    opt2.nOptLen = sizeof(keepAlive);
    socketOptions.push_back(opt2);
    
    EXPECT_EQ(socketOptions.size(), 2);
    EXPECT_EQ(socketOptions[0].nOptName, SO_REUSEADDR);
    EXPECT_EQ(socketOptions[1].nOptName, SO_KEEPALIVE);
}

// Test memory layout and alignment
TEST_F(SDNetOptTest, MemoryLayoutAndAlignment) {
    // Test that structures are properly aligned
    SNetLinOptQueueSize queueSize;
    
    // Verify that we can take addresses of all members
    volatile INT32* recv = &queueSize.nRecvBufSize;
    volatile INT32* send = &queueSize.nSendBufSize;
    volatile INT32* req = &queueSize.nRequestQueueSize;
    volatile INT32* event = &queueSize.nEventQueueSize;
    
    // Basic sanity checks that addresses are different
    EXPECT_NE(recv, send);
    EXPECT_NE(send, req);
    EXPECT_NE(req, event);
    
    // Verify alignment (should be at least 4-byte aligned for INT32)
    EXPECT_EQ(reinterpret_cast<uintptr_t>(recv) % sizeof(INT32), 0);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(send) % sizeof(INT32), 0);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(req) % sizeof(INT32), 0);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(event) % sizeof(INT32), 0);
}