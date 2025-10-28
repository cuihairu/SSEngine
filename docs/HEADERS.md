# 头文件索引

本页按照 include/ssengine 的目录结构梳理公共头文件的职责。每一节列出主功能及常见依赖，方便在实现、测试或文档之间跳转。

## 核心基座
- `sdbase.h`：定义模块引用计数接口 `ISSBase` 以及 `SSSVersion` 版本结构；所有模块均以此为基类。
- `sdconfig.h` / `sdmacros.h` / `sdconfigure.h`：平台/编译期宏开关集合，提供 `WINDOWS`、`LINUX` 特性裁剪与编译选项描述。
- `sdtype.h`：统一的基础类型别名、`SDAPI/SSAPI` 调用约定、句柄与错误码常量。
- `vercheck.h`：模块版本兼容检查辅助函数。
- `sscp.h`：SSCP 框架入口，聚合模块获取/日志设置接口。

## 并发与同步
- `sdthread.h`：线程封装与线程创建 API。
- `sdthreadctrl.h` / `sdthreadpool.h`：线程控制器与线程池抽象。
- `sdmutex.h` / `sdcondition.h` / `sdlock.h`：互斥量、条件变量、RAII 锁封装。
- `sdatomic.h`：跨平台原子操作集合。

## 定时与时间
- `sdtime.h`：时间戳、时间差、sleep 等基础工具。
- `sdtimer.h`：基于回调的定时器调度接口。

## 容器、算法与内存
- `sdmemorypool.h` / `sdobjectpool.h`：内存池与对象池实现。
- `sdhashmap.h` / `sdindexer.h` / `sdidpool.h` / `sdloopbuffer.h` / `sdserverid.h`：ID 管理、环形缓冲、哈希与索引容器。
- `sdutil.h` / `sdmath.h` / `sdrandom.h`：常用杂项工具、数学与随机数。
- `sdstring.h` / `sdutf8.h` / `sdtranslate.h`：字符串处理、编码转换及多语言支持。
- `algorithm/*`：封装在 `include/ssengine/algorithm` 目录下的通用算法实现（例如排序、查找辅助），由 `sdutil.h` 等头按需包含。
- `detail/*`：位于 `include/ssengine/detail` 的内部实现细节，供同目录头文件引用，不作为公开 API。

## 文件、流与序列化
- `sdfile.h` / `sdfilemapping.h` / `sdcsvfile.h`：文件操作、内存映射文件以及 CSV 帮助类。
- `sddir.h`：目录遍历与创建工具。
- `sddatastream.h` / `sddataqueue.h` / `sdfifo.h` / `sdloopbuffer.h`：数据流读写缓存、环形队列。
- `sdshm.h` / `sdshmem.h`：共享内存抽象与跨进程映射。

## 网络通信
- `sdnet.h`：核心网络模块接口，包括 `ISSNet`、`ISSConnection`、`ISSConnector`、`ISSSession`、`ISSListener` 等。
- `sdnetopt.h`：网络模块可选项（连接数、事件队列、套接字选项透传）。
- `sdpkg.h`：16/32 位包头、打包拆包工具及默认包解析器 `CSDPacketParser`。
- `sdnet_ver.h`：网络模块版本常量。
- `sdnetutils.h`：Inet/字节序工具、本机地址枚举。

## 管道与 IPC
- `sdpipe.h`：管道模块接口，定义 `ISSPipeModule`、`ISSPipe` 与 `ISSPipeSink` 等抽象，提供连接复用、上报回调以及白名单支持。

## 日志与调试
- `sdlogger.h`：日志模块接口与文件/UDP/TCP Logger 的工厂函数。
- `sddebug.h` / `sddebugviewer.h`：调试输出、调试变量注册与可视化。

## 控制台与工具
- `sdconsole.h`：命令行控制台、颜色输出、键盘 hook 等接口。
- `sdprocess.h`：进程创建、终止与属性查询。
- `sdtranslate.h`：翻译/本地化资源帮助。

## 数据库
- `sddb.h` / `sddb_ver.h`：数据库模块接口、命令执行与版本信息。

## 系统信息
- `sdsysteminfo.h`：硬件、OS、网络适配器与运行环境信息采集。

## 聚合入口与历史兼容
- `sdu.h`：工具库聚合入口，便于一次性包含全部常用 S* 工具。
- `sdconfigure.h` / `sdlimit.h` 等：兼容旧版接口及配置边界常量。

> **说明**：`include/ssengine/detail` 与 `include/ssengine/algorithm` 下的头文件属于内部实现或模板工具，并不直接对外暴露，但在阅读具体模块实现时可作为参考。
