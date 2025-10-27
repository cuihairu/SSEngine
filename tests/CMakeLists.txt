include(FetchContent)

# GoogleTest via FetchContent (fetched at configure/build time)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# Allow users to disable at configure time
option(SSE_USE_SYSTEM_GTEST "Use system-installed GoogleTest" OFF)
if (SSE_USE_SYSTEM_GTEST)
  find_package(GTest REQUIRED)
else()
  FetchContent_MakeAvailable(googletest)
endif()

# Tests target
add_executable(sse_tests
  test_sdlogger.cpp
  test_sdnet.cpp
  test_sdnetutils.cpp
  test_sdmutex.cpp
  test_sdtime.cpp
  test_sdfile.cpp
  test_sdatomic.cpp
  test_sdnet_roundtrip.cpp
  test_sdnet_sdpkg.cpp
  test_sdpipe_roundtrip.cpp
  test_sdpipe_replace.cpp
  test_sdpipe_remove.cpp
  test_sdnet_reconnect.cpp
  test_sdnet_close.cpp
)

target_link_libraries(sse_tests PRIVATE
  sdlogger
  sdu
  sdnet
  sdpipe
)

if (SSE_USE_SYSTEM_GTEST)
  target_link_libraries(sse_tests PRIVATE GTest::gtest GTest::gtest_main)
else()
  target_link_libraries(sse_tests PRIVATE GTest::gtest GTest::gtest_main)
endif()

# C++17
set_property(TARGET sse_tests PROPERTY CXX_STANDARD 17)
set_property(TARGET sse_tests PROPERTY CXX_STANDARD_REQUIRED ON)

include(GoogleTest)
gtest_discover_tests(sse_tests)
