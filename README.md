# SSEngine (Re-implementation)

本仓库用于在保持相同头文件接口的前提下，重写并替换原有 Windows SDK（win32/win64 下的预编译库）。目标平台：Windows x86/x64，Linux x86_64，macOS (x86_64/arm64)。

注意：win32/win64 目录中的头文件与库仍然保留，只作为对照测试与行为基准，不再直接参与后续构建。

## 目录结构
- include/        统一的公共头文件（当前从 win64/include 同步，后续保持为权威头）
- src/            各模块源码（先实现 Windows，后实现 Linux，最后 macOS）
- win32/, win64/  供应商二进制与头文件（对照用）
- CMakeLists.txt  顶层 CMake，工程使用 CMake 统一管理

## 模块清单（按优先级）
1) sdlogger    文件/UDP/TCP 日志器 + 工厂导出
2) sdu         常用工具（线程/锁/时间/文件/原子/网络字节序等）
3) sdnet       高性能网络（Windows: IOCP；Linux: epoll；macOS: kqueue）
4) sdpipe      基于 sdnet 的消息/管道抽象
5) sdconsole   Windows 控制台（固定区域/滚动区/颜色/键盘监听）
6) sddb        DB 会话/命令/结果集（第一阶段实现 Mock/最小可用语义）
7) sdgate, sdsysteminfo, sddebugviewer  如业务需要再实现

## 开发路线图
- 第 1 阶段（Windows）
  - 搭建工程脚手架与对照测试框架
  - 优先实现 sdlogger + sdu（最小可用子集）
  - 实现 Windows sdnet（IOCP）和 sdpipe，补充金测用例（与供应商库对拍）
  - 实现 sdconsole 与 sddb(Mock) 的最小语义集
  - 在 Windows CI 上按模块逐步对比验证（行为/错误码/回调顺序/边界场景）
- 第 2 阶段（Linux）
  - 复用头文件与接口，落地 epoll 版本的 sdnet 与 sdpipe，其余模块适配
  - Linux CI 对拍测试
- 第 3 阶段（macOS）
  - 复用同一套头文件，落地 kqueue 版本的 sdnet 与 sdpipe
  - macOS CI 对拍测试

## 对拍与测试策略
- 同进程同时加载“供应商库”和“自研库”，用相同用例驱动，比较：
  - 返回值与错误码（如 ESDNetErrCode、ESDPipeCode、SDDB 错误）
  - 回调顺序与时序窗口（Run 步进、粘包/半包、异常路径）
  - 地址/字符串/长度/缓冲区空间等可观察状态
- 不跑通对拍即不替换。性能回归另行评估（连接数、吞吐、CPU）。

## 已知兼容性事项
- sdsysteminfo.h/sddebugviewer.h 使用的 `SSSCPVersion` 类型在仓库内缺失定义，后续将以兼容头或别名方式补齐（不改变原头文件名与接口）。
- 32/64 位库命名在 win32/win64 不一致（ss*/sd* 前缀），替换时需分别生成。
- Windows 动态导出名将通过 .def 控制确保 ABI 一致；静态库作为中间形态可用于单元测试。

## 构建（脚手架）
- Windows（后续补齐 CI 脚本与编译说明）
  ```pwsh
  cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
  cmake --build build --config Release --target sdlogger
  ```
- 其他平台当前仅生成占位库，待实现对应平台逻辑后再启用。

## 变更记录（本次）
- 初始化 Git 仓库，关联远程 cuihairu/SSEngine
- 新增 CMake 脚手架：include/ 与 src/，并添加占位目标
- 提供 sdlogger 的最小占位实现与工厂函数（后续替换为真实实现）
- 规划分阶段目标与测试策略

## 下一步计划
1) 在 Windows 下完善 sdlogger 的文件/UDP/TCP 实现与行为对拍测试
2) 实现 sdu 最小子集（线程/锁/时间/文件/网络字节序/原子 等）
3) 起步 sdnet(IOCP) + sdpipe 的骨架与关键路径测试
4) 整合 Windows CI（构建、运行对拍测试）

