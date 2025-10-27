#include <gtest/gtest.h>
#include "ssengine/sdnetutils.h"
#include "ssengine/sdtype.h"

using namespace SSCP;

TEST(sdnetutils, inet_pton_ntop_ipv4_roundtrip) {
    const char* ipstr = "127.0.0.1";
    in_addr ia{};
    ASSERT_TRUE(SDNetInetPton(AF_INET, ipstr, &ia));
    char buf[64] = {0};
    ASSERT_NE(nullptr, SDNetInetNtop(AF_INET, &ia, buf, sizeof(buf)));
    EXPECT_STREQ(buf, ipstr);
}

TEST(sdnetutils, get_host_name_nonempty) {
    auto name = SDGetHostName();
    EXPECT_FALSE(name.empty());
}
