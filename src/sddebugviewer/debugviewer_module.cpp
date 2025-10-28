#include "ssengine/sddebugviewer.h"

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>

namespace SSCP {

namespace {

enum class EntryType {
    Int32,
    CharPtr,
    Double,
    Float,
    Word,
    Dword,
    Uint
};

struct Entry {
    EntryType type;
    void* address;
};

} // namespace

class DebugViewerModule : public ISSDebugViewer {
public:
    DebugViewerModule() : m_ref(1) {}
    ~DebugViewerModule() override = default;

    void SSAPI Register(const char* pName, INT32* pAddress) override {
        registerEntry(pName, pAddress, EntryType::Int32);
    }

    void SSAPI Register(const char* pName, char* pAddress) override {
        registerEntry(pName, pAddress, EntryType::CharPtr);
    }

    void SSAPI Register(const char* pName, double* pAddress) override {
        registerEntry(pName, pAddress, EntryType::Double);
    }

    void SSAPI Register(const char* pName, float* pAddress) override {
        registerEntry(pName, pAddress, EntryType::Float);
    }

    void SSAPI Register(const char* pName, WORD* pAddress) override {
        registerEntry(pName, pAddress, EntryType::Word);
    }

    void SSAPI Register(const char* pName, DWORD* pAddress) override {
        registerEntry(pName, pAddress, EntryType::Dword);
    }

    void SSAPI Register(const char* pName, UINT* pAddress) override {
        registerEntry(pName, pAddress, EntryType::Uint);
    }

    void SSAPI UnRegister(const char* pName) override {
        if (!pName) {
            return;
        }
        std::lock_guard<std::mutex> lk(m_mutex);
        m_entries.erase(pName);
    }

    void SSAPI AddRef(void) override {
        m_ref.fetch_add(1);
    }

    UINT32 SSAPI QueryRef(void) override {
        return m_ref.load();
    }

    void SSAPI Release(void) override {
        if (m_ref.fetch_sub(1) == 1) {
            delete this;
        }
    }

    SSSVersion SSAPI GetVersion(void) override {
        return SDDEBUGVIEWER_VERSION;
    }

    const char* SSAPI GetModuleName(void) override {
        return SDDEBUGVIEWER_MODULENAME;
    }

private:
    void registerEntry(const char* name, void* address, EntryType type) {
        if (!name || !address) {
            return;
        }
        std::lock_guard<std::mutex> lk(m_mutex);
        m_entries[std::string(name)] = Entry{type, address};
    }

private:
    std::atomic<UINT32> m_ref;
    std::mutex m_mutex;
    std::unordered_map<std::string, Entry> m_entries;
};

ISSDebugViewer* SSAPI SDDebugViewerGetModule(const SSSCPVersion* /*pstVersion*/) {
    return new DebugViewerModule();
}

} // namespace SSCP

