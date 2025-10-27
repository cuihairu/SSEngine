# Module libraries scaffold. Windows implementation will come first.

# Always expose headers to consumers
set(PUBLIC_INC ${SSE_PUBLIC_INCLUDE_DIR})

# sdlogger (scaffold)
add_library(sdlogger STATIC
  sdlogger/file_logger_win.cpp
  sdlogger/udp_logger_win.cpp
  sdlogger/tcp_logger_win.cpp
)

target_include_directories(sdlogger PUBLIC ${PUBLIC_INC})

# Placeholder targets for other modules; sources will be added in subsequent steps
add_library(sdu STATIC sdu/placeholder.cpp)
target_include_directories(sdu PUBLIC ${PUBLIC_INC})

add_library(sdnet STATIC sdnet/placeholder.cpp)
target_include_directories(sdnet PUBLIC ${PUBLIC_INC})

add_library(sdpipe STATIC sdpipe/placeholder.cpp)
target_include_directories(sdpipe PUBLIC ${PUBLIC_INC})

add_library(sddb STATIC sddb/placeholder.cpp)
target_include_directories(sddb PUBLIC ${PUBLIC_INC})

add_library(sdconsole STATIC sdconsole/placeholder.cpp)
target_include_directories(sdconsole PUBLIC ${PUBLIC_INC})

add_library(sdgate STATIC sdgate/placeholder.cpp)
target_include_directories(sdgate PUBLIC ${PUBLIC_INC})

add_library(sdsysteminfo STATIC sdsysteminfo/placeholder.cpp)
target_include_directories(sdsysteminfo PUBLIC ${PUBLIC_INC})

add_library(sddebugviewer STATIC sddebugviewer/placeholder.cpp)
target_include_directories(sddebugviewer PUBLIC ${PUBLIC_INC})

# Platform gating can be added later when real sources are present
