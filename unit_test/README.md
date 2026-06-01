# common_util Unit Tests

基于 [GoogleTest](https://github.com/google/googletest) 的单元测试集，覆盖 `src/common_util` 下所有公开模块。

## 目录结构

```
unit_test/
├── CMakeLists.txt        # 通过 FetchContent 拉取 GoogleTest，构建测试可执行文件
├── test_main.cpp         # 测试入口（初始化日志回调 + InitGoogleTest）
├── test_strutil.cpp      # 字符串处理：strip / split / join / s2ws ...
├── test_strfmt.cpp       # 字符串格式化：fmt_uint / fmt_filesize / to_hex ...
├── test_verutil.cpp      # 版本号工具：is_version / compare_version
├── test_algoutil.cpp     # 通用算法：clamp / in_range
├── test_timeutil.cpp     # 时间戳与时区：timestamp / clocktime / cpu_clocktime
├── test_datetime.cpp     # 日期时间：构造 / format / 加减运算
├── test_timecount.cpp    # 计时器：steady_timecounter / cpu_timecounter
├── test_timer.cpp        # 定时器：set_timeout / cutl::timer
├── test_filepath.cpp     # 路径：join / dirname / basename / extension
├── test_fileutil.cpp     # 文件 IO：createfile / list_files / copyfile ...
├── test_lrucache.cpp     # LRU 缓存
├── test_bitmap.cpp       # 位图：bitmap / dynamic_bitmap / roaring_bitmap
├── test_bloomfilter.cpp  # 布隆过滤器
├── test_hash.cpp         # 各类 hash 函数
├── test_hyperloglog.cpp  # 基数估计
├── test_sysutil.cpp      # 系统工具：platform / endian / system / getenv
├── test_threadutil.cpp   # 线程 tid / 线程名
├── test_threadpool.cpp   # 线程池
├── test_eventloop.cpp    # 事件循环（单线程 / 多线程）
├── test_color.cpp        # ARGB 颜色
├── test_observer.cpp     # 观察者模式
├── test_statemachine.cpp # 状态机
├── test_singleton.cpp    # 单例模式宏
├── test_print.cpp        # 控制台打印
├── test_config.cpp       # library_init / 版本信息
└── test_dlloader.cpp     # 动态库加载（按需跳过）
```

## 编译并运行

测试默认不编译，需要在配置阶段显式开启 `BUILD_UNIT_TEST=ON`：

```bash
cmake -S . -B build_ut -DBUILD_UNIT_TEST=ON
cmake --build build_ut -j
ctest --test-dir build_ut --output-on-failure
```

也可以直接运行可执行文件，享受 GoogleTest 的过滤参数：

```bash
./build_ut/unit_test/common_util_unit_test --gtest_filter='StrUtilTest.*'
```

## 关键设计

* **GoogleTest 通过 FetchContent 拉取**，无需系统级安装。
* **C++ 标准对齐主库**：当主项目以 `CXX_STANDARD_11=ON` 配置时，自动切换到 C++11，并使用最后一版兼容 C++11 的 GoogleTest（`release-1.12.1`）；否则跟随主库默认值（通常 C++17），避免 `__cplusplus` 分支不一致引发的链接错误。
* **临时文件**：文件系统相关的测试都会在系统临时目录下建立独立的目录，结束时自动清理。
* **Event loop / Timer**：涉及多线程或阻塞 API 时统一在独立线程中调用，并使用短轮询等待状态稳定，避免测试 hang。
* **dlloader**：测试会先在系统中查找 `libm.so.6` / `libc.so.6` 等已知库；若都不存在则直接 `GTEST_SKIP`。

## 历史已修复的库 Bug（已纳入回归测试）

为编写这套测试时发现并已经在库中修复的若干问题：

| 模块 | 问题 | 修复方式 |
|------|------|---------|
| `lrucache.h` | 直接使用 `std::function` 但未 `#include <functional>` | 头文件中补上 `<functional>` |
| `lru_cache::exist()` | `const` 方法锁了非 `mutable` 的 `mutex_`，部分编译器会拒绝编译 | 把 `mutex_` 声明为 `mutable std::mutex` |
| `lru_cache` 构造 | `head_/tail_` 未初始化，析构时可能产生悬垂指针 | 构造函数初始化为 `nullptr` |
| `datetime::second/min/hour/day` | `static constexpr` 缺类外定义，`EXPECT_EQ` 等 odr-use 时链接报错 | 在 `datetime.cpp` 添加类外再声明 |
| `to_hex(uint8_t*, size_t, ...)` | 临时变量按 `signed char` 处理，字节 ≥ 0x80 结果错乱 | 改为 `uint8_t` |
| `dlloader::~dlloader` | 加载失败时仍调用 `dlclose(nullptr)`，Unix 下 UB | 析构时判空再释放 |

对应的测试断言已经收紧到正常预期（不再依赖任何"绕开库 bug"的写法）。
