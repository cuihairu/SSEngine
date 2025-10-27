# 头文件分类与用途

本文件梳理 include/ 中头文件的职责，作为实现与测试的导航。

- 核心/版本
  - sdbase.h: 基础接口 ISSBase、模块版本结构 SSSVersion
  - sdconfig.h: 平台/特性宏（WINDOWS/LINUX 等）
  - vercheck.h: 版本比较工具声明
  - sscp.h: SSCP 公共入口

- 通用工具（sdu 聚合与组件）
  - include/include/sdu.h: 工具库聚合入口，包含所有子组件头
  - include/include/sdtype.h: 平台类型、SSAPI 宏、句柄
  - include/include/sdnetutils.h: 网络工具（inet/字节序/本机地址）
  - include/include/sdthread.h / sdmutex.h / sdcondition.h: 线程、锁、条件变量
  - include/include/sdtime.h / sdtimer.h: 时间与定时器封装
  - include/include/sdfile.h / sdfilemapping.h: 文件与映射
  - include/include/sdatomic.h / sdlock.h: 原子与锁
  - include/include/sdstring.h / sdrandom.h / sdmemorypool.h / sdobjectpool.h / sdindexer.h / sdhashmap.h 等：字符串、随机、内存池、容器等
  - include/include/detail/*: 对象分配器、配置解析等内部工具

- 网络（sdnet）
  - sdnet.h: 网络核心接口（ISSNet/ISSConnection/ISSConnector/ISSListener/ISSSession 等）；错误码与 I/O 模型
  - sdnetopt.h: 扩展选项（连接上限、队列大小、sockopt 透传）
  - sdpkg.h: 包头16/32结构、校验与包长判定、默认包解析器 CSDPacketParser
  - sdnet_ver.h: 模块版本

- 管道（sdpipe）
  - sdpipe.h: ISSPipeModule/ISSPipe/ISSPipeSink 等；状态上报、连接管理、日志设置

- 日志（sdlogger）
  - sdlogger.h: ISSLogger 抽象与文件/UDP/TCP Logger 接口 + 工厂；日志级别位

- 控制台（sdconsole）
  - sdconsole.h: 控制台固定区/滚动区/颜色/键盘监听/Hook；工厂与日志设置

- 数据库（sddb）
  - sddb.h: ISSDBModule/ISSDBSession/ISSDBCommand/ISSDBConnection/ISSDBRecordSet；工厂与日志设置
  - sddb_ver.h: 模块版本

- Gate（sdgate）
  - sdgate.h: ISSGate、ISSServerConnection/Session、ISSClientConnection/Session/Factory；监听、转发、进入/离开语义

- 系统信息/调试
  - sdsysteminfo.h: ISSSystemInfo（CPU/GPU/内存/DirectX/机器ID/系统信息）；依赖 SSSCPVersion（后续以兼容别名补齐）
  - sddebugviewer.h: ISSDebugViewer（调试变量注册）；同样引用 SSSCPVersion

- 线程控制/共享内存
  - sdthreadctrl.h: ISSThread/ISSThreadCtrl 控制类接口
  - sdshm.h: 共享内存抽象

已知兼容性
- sdsysteminfo.h/sddebugviewer.h 使用的 `SSSCPVersion` 类型在现有头集中缺失定义，将以兼容方式提供别名（不修改原头文件 API）。

