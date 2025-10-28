#include <gtest/gtest.h>

#include "ssengine/sdtranslate.h"

using namespace SSCP;

TEST(sdtranslate, basic_copy_without_iconv) {
#if SDU_WITH_LIBICONV
    GTEST_SKIP() << "iconv path not enabled in tests";
#else
    CSDTranslate translator;
    ASSERT_TRUE(translator.Init("UTF-8", "UTF-8"));
    char dest[16] = {};
    size_t written = translator.Translate(const_cast<char*>("hi"), 2, dest, sizeof(dest));
    EXPECT_EQ(written, 2u);
    dest[written] = '\0';
    EXPECT_STREQ(dest, "hi");
#endif
}
