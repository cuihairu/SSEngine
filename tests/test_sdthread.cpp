#include <gtest/gtest.h>
#include "ssengine/sdthread.h"
#include "ssengine/sdtype.h"

using namespace SSCP;

TEST(sdthread, get_thread_id_returns_something) {
#if defined(_WIN32)
    auto tid = SDGetThreadId();
    EXPECT_NE(tid, 0u);
#else
    GTEST_SKIP() << "sdthread non-Windows implementation pending";
#endif
}

#if defined(_WIN32)
namespace {
class MyThread : public CSDThread {
public:
    void SSAPI ThrdProc() override { ran = true; }
    bool ran{false};
};

SDTHREAD_DECLARE(fproc)(void* arg) {
    bool* flag = static_cast<bool*>(arg);
    if (flag) *flag = true;
    SDTHREAD_RETURN(0);
}
}

TEST(sdthread, class_thread_start_and_wait) {
    MyThread t;
    ASSERT_TRUE(t.Start(FALSE));
    t.Wait();
    EXPECT_TRUE(t.ran);
}

TEST(sdthread, function_thread_start_and_wait) {
    BOOL ran = FALSE;
    SDTHREADID tid = 0;
    SDHANDLE h = SDCreateThread(nullptr, &fproc, &ran, &tid, FALSE);
    ASSERT_NE(h, SDINVALID_HANDLE);
    EXPECT_NE(tid, 0u);
    EXPECT_EQ(0, SDThreadWait(h));
    SDThreadCloseHandle(h);
    EXPECT_TRUE(ran);
}
#endif

