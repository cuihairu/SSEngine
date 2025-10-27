#include <gtest/gtest.h>
#include "sdnet.h"
#include "sdnet_ver.h"

using namespace SSCP;

TEST(sdnet, factory_not_null) {
    auto* net = SSNetGetModule(&SDNET_MODULE_VERSION);
    ASSERT_NE(net, nullptr);
    EXPECT_STREQ(net->GetModuleName(), SDNET_MODULENAME);
    auto ver = net->GetVersion();
    EXPECT_EQ(ver.wMajorVersion, SDNET_MODULE_VERSION.wMajorVersion);
    net->Release();
}
