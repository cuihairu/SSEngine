#include "ssengine/sdconfig.h"

#ifdef WINDOWS
#  include <windows.h>
#else
#  include <unistd.h>
#endif

namespace SSCP {

const char* GetPlatformName() {
#ifdef WINDOWS
    return "windows";
#elif defined(__APPLE__)
    return "darwin";
#else
    return "linux";
#endif
}

} // namespace SSCP

