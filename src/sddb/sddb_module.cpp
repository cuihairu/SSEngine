#include "ssengine/sddb.h"
#include "ssengine/sddb_ver.h"
#include "ssengine/sdlogger.h"

#include <atomic>
#include <deque>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

namespace SSCP {

namespace {

ISSLogger* g_sddbLogger = nullptr;
UINT32 g_sddbLogLevel = LOGLV_INFO | LOGLV_WARN | LOGLV_CRITICAL;
std::mutex g_loggerMutex;

void logInfo(const std::string& text) {
    std::lock_guard<std::mutex> lk(g_loggerMutex);
    if (g_sddbLogger && (g_sddbLogLevel & LOGLV_INFO)) {
        g_sddbLogger->LogText(text.c_str());
    }
}

UINT32 escapeString(const CHAR* pSrc, INT32 nSrcSize,
                    CHAR* pDest, INT32 nDstSize) {
    if (!pSrc || !pDest || nSrcSize <= 0 || nDstSize <= 0) {
        if (pDest && nDstSize > 0) {
            pDest[0] = '\0';
        }
        return 0;
    }
    INT32 written = 0;
    for (INT32 i = 0; i < nSrcSize && pSrc[i] != '\0'; ++i) {
        char c = pSrc[i];
        if (written >= nDstSize - 1) break;
        if (c == '\'' || c == '\"' || c == '\\') {
            if (written + 1 >= nDstSize - 1) break;
            pDest[written++] = '\\';
        }
        pDest[written++] = c;
    }
    pDest[written] = '\0';
    return static_cast<UINT32>(written);
}

class MockRecordSet : public ISSDBRecordSet {
public:
    MockRecordSet() : m_retrieved(false) {}
    ~MockRecordSet() override = default;

    UINT32 SSAPI GetRecordCount(void) override { return 0; }
    UINT32 SSAPI GetFieldCount(void) override { return 0; }
    bool SSAPI GetRecord(void) override {
        if (m_retrieved) return false;
        m_retrieved = true;
        return false;
    }
    const CHAR* SSAPI GetFieldValue(UINT32) override { return nullptr; }
    INT32 SSAPI GetFieldLength(UINT32) override { return 0; }
    void SSAPI Release(void) override { delete this; }
    const CHAR* SSAPI GetFieldValueByName(const CHAR*) override { return nullptr; }
    INT32 SSAPI GetFieldLengthByName(const CHAR*) override { return 0; }

private:
    bool m_retrieved;
};

class MockConnection : public ISSDBConnection {
public:
    MockConnection() = default;
    ~MockConnection() override = default;

    bool SSAPI CheckConnection() override { return true; }

    UINT32 SSAPI EscapeString(const CHAR* pSrc, INT32 nSrcSize,
                              CHAR* pDest, INT32 nDstSize) override {
        return escapeString(pSrc, nSrcSize, pDest, nDstSize);
    }

    INT32 SSAPI ExecuteSql(const CHAR* pSQL, UINT64* pInsertId = nullptr) override {
        if (pInsertId) {
            *pInsertId = 0;
        }
        if (pSQL) {
            logInfo(std::string("ExecuteSql: ") + pSQL);
        }
        return SDDB_SUCCESS;
    }

    INT32 SSAPI ExecuteSqlRs(const CHAR* pSQL, ISSDBRecordSet** ppoRs) override {
        if (pSQL) {
            logInfo(std::string("ExecuteSqlRs: ") + pSQL);
        }
        if (ppoRs) {
            *ppoRs = nullptr;
        }
        return SDDB_NO_RECORDSET;
    }

    void SSAPI BeginTransaction() override {}
    void SSAPI CommitTransaction() override {}
    void SSAPI RollbackTransaction() override {}
    bool SSAPI CreateDB(const CHAR*, bool, const CHAR*) override { return true; }
    bool SSAPI SelectDB(const CHAR*) override { return true; }
    void SSAPI Release() override { delete this; }
};

class MockSession : public ISSDBSession {
public:
    MockSession()
        : m_connection(new MockConnection()) {}

    ~MockSession() override {
        clearPending();
        if (m_connection) {
            m_connection->Release();
            m_connection = nullptr;
        }
    }

    UINT32 SSAPI EscapeString(const CHAR* pSrc, INT32 nSrcSize,
                              CHAR* pDest, INT32 nDstSize, INT32 /*timeout*/ = -1) override {
        return escapeString(pSrc, nSrcSize, pDest, nDstSize);
    }

    INT32 SSAPI ExecuteSql(const CHAR* pSQL, UINT64* pInsertId = nullptr, INT32 /*timeout*/ = -1) override {
        return m_connection ? m_connection->ExecuteSql(pSQL, pInsertId) : SDDB_ERR_CONN;
    }

    INT32 SSAPI ExecuteSqlRs(const CHAR* pSQL, ISSDBRecordSet** ppoRs, INT32 /*timeout*/ = -1) override {
        return m_connection ? m_connection->ExecuteSqlRs(pSQL, ppoRs) : SDDB_ERR_CONN;
    }

    bool SSAPI CreateDB(const CHAR*, bool, const CHAR*, INT32 /*timeout*/ = -1) override {
        return true;
    }

    bool SSAPI SelectDB(const CHAR*, INT32 /*timeout*/ = -1) override {
        return true;
    }

    bool SSAPI AddDBCommand(ISSDBCommand* poDBCommand) override {
        return enqueueCommand(poDBCommand);
    }

    bool SSAPI QuickAddDBCommand(ISSDBCommand* poDBCommand) override {
        return enqueueCommand(poDBCommand);
    }

    bool SSAPI Run(INT32 nCount = -1) override {
        INT32 processed = 0;
        while (nCount < 0 || processed < nCount) {
            ISSDBCommand* cmd = nullptr;
            {
                std::lock_guard<std::mutex> lk(m_mutex);
                if (m_completed.empty()) {
                    break;
                }
                cmd = m_completed.front();
                m_completed.pop_front();
            }
            if (!cmd) {
                continue;
            }
            cmd->OnExecuted();
            cmd->Release();
            ++processed;
        }
        return true;
    }

    UINT32 SSAPI GetDBCommandCount() override {
        std::lock_guard<std::mutex> lk(m_mutex);
        return static_cast<UINT32>(m_completed.size());
    }

private:
    bool enqueueCommand(ISSDBCommand* poDBCommand) {
        if (!poDBCommand) {
            return false;
        }
        if (!m_connection) {
            return false;
        }
        poDBCommand->OnExecuteSql(m_connection);
        std::lock_guard<std::mutex> lk(m_mutex);
        m_completed.push_back(poDBCommand);
        return true;
    }

    void clearPending() {
        std::lock_guard<std::mutex> lk(m_mutex);
        while (!m_completed.empty()) {
            ISSDBCommand* cmd = m_completed.front();
            m_completed.pop_front();
            if (cmd) {
                cmd->Release();
            }
        }
    }

private:
    MockConnection* m_connection;
    std::deque<ISSDBCommand*> m_completed;
    std::mutex m_mutex;
};

class MockDBModule : public ISSDBModule {
public:
    MockDBModule() : m_ref(1) {}
    ~MockDBModule() override {
        std::lock_guard<std::mutex> lk(m_mutex);
        for (MockSession* session : m_sessions) {
            if (session) {
                delete session;
            }
        }
        m_sessions.clear();
    }

    void SSAPI AddRef(void) override { m_ref.fetch_add(1); }

    UINT32 SSAPI QueryRef(void) override { return m_ref.load(); }

    void SSAPI Release(void) override {
        if (m_ref.fetch_sub(1) == 1) {
            delete this;
        }
    }

    SSSVersion SSAPI GetVersion(void) override { return SDDB_VERSION; }
    const char* SSAPI GetModuleName(void) override { return SDDB_MODULENAME; }

    ISSDBSession* SSAPI GetDBSession(const CHAR* /*pszConfigString*/) override {
        return createSession();
    }

    ISSDBSession* SSAPI GetDBSession(SDDBAccount* /*pstDBAccount*/) override {
        return createSession();
    }

    ISSDBSession* SSAPI GetDBSession(SDDBAccount* /*pstDBAccount*/, UINT32 /*coreSize*/, UINT32 /*maxSize*/) override {
        return createSession();
    }

    void SSAPI Close(ISSDBSession* pDBSession) override {
        if (!pDBSession) return;
        std::lock_guard<std::mutex> lk(m_mutex);
        auto it = m_sessions.begin();
        while (it != m_sessions.end()) {
            if (*it == pDBSession) {
                delete *it;
                it = m_sessions.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    ISSDBSession* createSession() {
        auto* session = new MockSession();
        std::lock_guard<std::mutex> lk(m_mutex);
        m_sessions.push_back(session);
        return session;
    }

private:
    std::atomic<UINT32> m_ref;
    std::mutex m_mutex;
    std::vector<MockSession*> m_sessions;
};

} // namespace

ISSDBModule* SSAPI SSDBGetModule(const SSSVersion* /*pstVersion*/) {
    return new MockDBModule();
}

bool SSAPI SSDBSetLogger(ISSLogger* poLogger, UINT32 dwLevel) {
    std::lock_guard<std::mutex> lk(g_loggerMutex);
    g_sddbLogger = poLogger;
    g_sddbLogLevel = dwLevel;
    return true;
}

} // namespace SSCP
