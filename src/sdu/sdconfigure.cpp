#include "ssengine/sdconfigure.h"

#include <unordered_map>

namespace SSCP {

namespace {

class DummyConfigure {
public:
    BOOL ReadConfig(const char*) { return TRUE; }
    BOOL WriteConfig(const char*) { return TRUE; }
    std::string GetProperty(const std::string& key) const {
        auto it = values.find(key);
        return it == values.end() ? std::string() : it->second;
    }
    void SetProperty(const char* key, const std::string& val) {
        if (key) {
            values[key] = val;
        }
    }

private:
    std::unordered_map<std::string, std::string> values;
};

} // namespace

template class CSDConfigureImpl<DummyConfigure>;

} // namespace SSCP
