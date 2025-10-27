cmake_minimum_required(VERSION 3.20)
project(SSEngine LANGUAGES CXX)

# Global settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SSE_BUILD_WINDOWS_IMPL "Enable Windows implementation builds" ON)
option(SSE_BUILD_LINUX_IMPL   "Enable Linux implementation builds"   OFF)
option(SSE_BUILD_MACOS_IMPL   "Enable macOS implementation builds"   OFF)

# Public headers (mirrored from vendor win64/include for now)
set(SSE_PUBLIC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SSE_PUBLIC_INCLUDE_DIRS
    ${SSE_PUBLIC_INCLUDE_DIR}
    ${SSE_PUBLIC_INCLUDE_DIR}/include
)

# Export compile definitions for platform macros used in headers
if (WIN32)
  add_compile_definitions(WINDOWS WIN32_LEAN_AND_MEAN)
else()
  add_compile_definitions(LINUX)
endif()

# Libraries (scaffold)
add_subdirectory(src)

# Tests
include(CTest)
if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
