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
  sdu/sdfile.cpp
  sdu/sdatomic.cpp
  sdu/sdfilemapping.cpp
  sdu/sdshmem.cpp
  sdu/sdprocess.cpp
  sdu/sdregex.cpp
  sdu/sdcsvfile.cpp
  sdu/sddatastream.cpp
  sdu/sdloopbuffer.cpp
  sdu/sdrandom.cpp
  sdu/sddataqueue.cpp
  sdu/sdconfig.cpp
  sdu/sdconfigure.cpp
  sdu/sdthreadctrl.cpp
  sdu/sdtimer.cpp
  sdu/sdtranslate.cpp
  sdu/sdstring.cpp
  sdu/sdthreadpool.cpp
  sdu/placeholder.cpp
)
target_include_directories(sdu PUBLIC ${PUBLIC_INCS})
target_compile_features(sdu PUBLIC cxx_std_17)
if (WIN32)
  target_link_libraries(sdu PRIVATE ws2_32)
endif()

add_library(sdnet STATIC)
if (WIN32)
  target_sources(sdnet PRIVATE sdnet/win_sdnet.cpp)
else()
  target_sources(sdnet PRIVATE sdnet/linux_sdnet.cpp)
endif()
target_include_directories(sdnet PUBLIC ${PUBLIC_INCS})
target_compile_features(sdnet PUBLIC cxx_std_17)
if (WIN32)
  target_link_libraries(sdnet PRIVATE ws2_32)
endif()

add_library(sdpipe STATIC
  sdpipe/pipe_module.cpp
)
target_include_directories(sdpipe PUBLIC ${PUBLIC_INCS})
target_link_libraries(sdpipe PUBLIC sdnet sdu)
target_compile_features(sdpipe PUBLIC cxx_std_17)

add_library(sdalgorithm STATIC
  algorithm/sdcrc.cpp
  algorithm/sddes.cpp
  algorithm/sdmd5.cpp
  algorithm/sdstrencrypt.cpp
)
target_include_directories(sdalgorithm PUBLIC ${PUBLIC_INCS})
target_compile_features(sdalgorithm PUBLIC cxx_std_17)

add_library(sddb STATIC sddb/sddb_module.cpp)
target_include_directories(sddb PUBLIC ${PUBLIC_INCS})
target_compile_features(sddb PUBLIC cxx_std_17)

add_library(sdconsole STATIC sdconsole/console_module.cpp)
target_include_directories(sdconsole PUBLIC ${PUBLIC_INCS})
target_compile_features(sdconsole PUBLIC cxx_std_17)

add_library(sdgate STATIC sdgate/gate_module.cpp)
target_include_directories(sdgate PUBLIC ${PUBLIC_INCS})
target_compile_features(sdgate PUBLIC cxx_std_17)

add_library(sdsysteminfo STATIC sdsysteminfo/systeminfo_module.cpp)
target_include_directories(sdsysteminfo PUBLIC ${PUBLIC_INCS})
target_compile_features(sdsysteminfo PUBLIC cxx_std_17)

add_library(sddebugviewer STATIC sddebugviewer/debugviewer_module.cpp)
target_include_directories(sddebugviewer PUBLIC ${PUBLIC_INCS})
target_compile_features(sddebugviewer PUBLIC cxx_std_17)

# Platform gating can be added later when real sources are present
