Roadmap / Status

Status Overview
- Headers: moved to include/ssengine/; all sources/tests updated to include via ssengine/...; copyrights updated to "Copyright (C) 2025 Cui Hairu. All rights reserved."
- Build/CI: CMake (C++17), GoogleTest + CTest; GitHub Actions on Windows/Linux/macOS; tests enabled on all 3 where applicable
- sdu (utilities)
  - Implemented: sdthread (Win + pthread), sdmutex (Win + pthread), sdcondition (Win + pthread), sdtime, sdfile, sdatomic (Win Interlocked + GCC/Clang __atomic), sdnetutils
  - Pending/optional: remaining headers in sdu.h (e.g., sdfilemapping, sdshmem, sdprocess, sdregex, etc.) if needed by upper modules
- sdlogger
  - Implemented: file logger (cross-platform), UDP/TCP logger (Windows); CSDLogger wrapper; tests (day/month rolling, UDP basic send, TCP connect-fail fallback)
  - Pending: UDP/TCP logger for Linux/macOS; level filtering (module-level mask) if required
- sdnet
  - Implemented: Windows (Winsock thread-based), Linux/macOS (POSIX sockets) with parser accumulation, Run-driven callbacks, ReConnect, SetBufferSize, GetSendBufFree (Win), DelaySend async, module defaults via SSNetSetOpt
  - Tests: roundtrip, sdpkg sticky/split, reconnect, server-close, client-close, delay_send roundtrip
  - Pending: refined error/close sequencing and error codes; performance model (IOCP/epoll/kqueue or send queue) if needed
- sdpipe
  - Implemented: sdpkg framing, AddConn/ReplaceConn/RemoveConn, AddListen, per-businessID sinks, Reporter (PIPE_SUCCESS/PIPE_DISCONNECT), IP whitelist (ReloadIPList/CheckIpValid, enforced on AddConn/accept), resource cleanup on destruction
  - Tests: roundtrip (with server echo), replace (sinks persist), remove (pipe erased), reporter success/disconnect, whitelist blocking
  - Pending: additional Reporter codes (e.g., PIPE_REPEAT_CONN), extended config (group reload/whitelist CIDR), metrics/backpressure
- sddb: placeholder (mock/real DB to be implemented later)
- sdconsole: placeholder (Windows/macOS/Linux support TBD)
- sdgate/sdsysteminfo/sddebugviewer: placeholder (alias SSSCPVersion added for header consistency)

Completed Tests (representative)
- sdnet: roundtrip; sdpkg sticky/split; reconnect; server-close and client-close terminate; delay_send roundtrip
- sdpipe: roundtrip (server echo); replace keeps sinks; remove removes pipe; reporter success/disconnect; whitelist AddConn blocking
- sdlogger: day/month rolling; UDP basic send; TCP connect-fail fallback
- sdu: thread/mutex/condition/time/file/atomic/netutils unit tests

Next Tasks (detailed)
- sdnet
  - Error and close sequencing
    - Ensure OnError vs OnTerminate ordering; call Release exactly once per session; unify behavior on explicit Disconnect vs remote close
    - Normalize error codes (NET_* mapping) for common socket errors (timeout/refused/reset)
    - Emit Terminated only once per connection; guard against duplicate events
  - Send/receive robustness and performance
    - Optional: implement queued DelaySend (single worker per connection or threadpool) to avoid per-call thread spawn
    - Optional: migrate Windows to IOCP and Linux/macOS to epoll/kqueue for high concurrency; keep Run-driven callback semantics
    - Backpressure: expose or simulate send buffer watermarks; refine GetSendBufFree (Linux/macOS)
  - SetOpt coverage: support more sockopts when needed (TCP_NODELAY, KEEPALIVE, REUSEPORT if applicable)
  - Timeouts: add configurable connect/send/recv timeouts; tests for timeout paths
  - Testing: inject failures (RST/half-open, partial sends), large payload framing, many-parallel connections; fuzz parser accumulation

- sdpipe
  - Reporter and state
    - Add more PIPE_* codes (e.g., PIPE_REPEAT_CONN, PIPE_REMOTEID_ERR mapping) and test their emission points
    - Ensure server/client sides report symmetrically on close/replace/remove
  - Config and whitelist
    - Extend ReloadPipeConfig to support groups/ids and dynamic changes without restart
    - Whitelist: accept CIDR ranges and comment formats; hot-reload tests
  - API behavior & safety
    - Validate message size limits and reject oversize; document thread-safety for SetSink/GetSink/Send
    - Optional buffering/backpressure per pipe; metrics (sent/recv counters)

- sdlogger
  - Implement UDP/TCP logger on Linux/macOS; tests with local sockets
  - Add internal level mask in CSDLogger and honor it; tests for filtering
  - Concurrency tests: multi-thread writes/rotation correctness; POSIX file perms/umask considerations

- sdu
  - Implement remaining modules as needed: sdfilemapping, sdshmem, sdprocess, sdregex, etc.
  - Add more cross-platform tests (file perms, time formatting, atomic stress)

- sddb
  - Phase 1: Mock session/command/recordset matching headers; async queue + Run-driven callbacks; unit tests
  - Phase 2: Real adapter (e.g., MySQL/ODBC); connection pool; integration tests gated in CI

- sdconsole
  - Phase 1: Windows implementation (fixed/scroll areas, color, input thread, Ctrl+C hook); tests
  - Phase 2: Cross-platform alternative (ncurses/backends), feature parity evaluation

- Cross-cutting & repo
  - Examples: small samples for sdnet echo, sdpipe business sink, sdlogger usage
  - Benchmarks: throughput/latency for sdnet/sdpipe on Windows/Linux/macOS
  - Tooling: address-sanitizer/ubsan builds on Linux/macOS; static analysis gates
  - Packaging: install targets, versioning (sdnet_ver etc.), release artifacts
  - Docs: module usage guides; migration note (include/ssengine path); public API stability statement

Migration Notes
- Public headers are under include/ssengine/; use #include <ssengine/xxx.h>. No legacy forwarding headers are provided by design.
