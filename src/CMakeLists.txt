# Module libraries scaffold. Windows implementation will come first.

# Always expose headers to consumers
set(PUBLIC_INCS ${SSE_PUBLIC_INCLUDE_DIRS})

# sdlogger (scaffold)
add_library(sdlogger STATIC
  sdlogger/file_logger_win.cpp
  sdlogger/udp_logger_win.cpp
  sdlogger/tcp_logger_win.cpp
  sdlogger/csdlogger.cpp
)

target_include_directories(sdlogger PUBLIC ${PUBLIC_INCS})
target_compile_features(sdlogger PUBLIC cxx_std_17)
if (WIN32)
  target_link_libraries(sdlogger PRIVATE ws2_32)
  target_compile_definitions(sdlogger PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS)
endif()

# Placeholder targets for other modules; sources will be added in subsequent steps
add_library(sdu STATIC
  sdu/sdnetutils.cpp
  sdu/sdthread.cpp
  sdu/sdmutex.cpp
  sdu/sdcondition.cpp
  sdu/sdtime.cpp
  sdu/placeholder.cpp
)
target_include_directories(sdu PUBLIC ${PUBLIC_INCS})
target_compile_features(sdu PUBLIC cxx_std_17)
if (WIN32)
  target_link_libraries(sdu PRIVATE ws2_32)
endif()

add_library(sdnet STATIC sdnet/win_sdnet.cpp)
target_include_directories(sdnet PUBLIC ${PUBLIC_INCS})
target_compile_features(sdnet PUBLIC cxx_std_17)
if (WIN32)
  target_link_libraries(sdnet PRIVATE ws2_32)
endif()

add_library(sdpipe STATIC sdpipe/placeholder.cpp)
target_include_directories(sdpipe PUBLIC ${PUBLIC_INCS})
target_compile_features(sdpipe PUBLIC cxx_std_17)

add_library(sddb STATIC sddb/placeholder.cpp)
target_include_directories(sddb PUBLIC ${PUBLIC_INCS})
target_compile_features(sddb PUBLIC cxx_std_17)

add_library(sdconsole STATIC sdconsole/placeholder.cpp)
target_include_directories(sdconsole PUBLIC ${PUBLIC_INCS})
target_compile_features(sdconsole PUBLIC cxx_std_17)

add_library(sdgate STATIC sdgate/placeholder.cpp)
target_include_directories(sdgate PUBLIC ${PUBLIC_INCS})
target_compile_features(sdgate PUBLIC cxx_std_17)

add_library(sdsysteminfo STATIC sdsysteminfo/placeholder.cpp)
target_include_directories(sdsysteminfo PUBLIC ${PUBLIC_INCS})
target_compile_features(sdsysteminfo PUBLIC cxx_std_17)

add_library(sddebugviewer STATIC sddebugviewer/placeholder.cpp)
target_include_directories(sddebugviewer PUBLIC ${PUBLIC_INCS})
target_compile_features(sddebugviewer PUBLIC cxx_std_17)

# Platform gating can be added later when real sources are present
