# 构建和运行

## Windows


## Unix-like

包含Linux/macOS等类Unix系统

1. 编译

```bash
./script/build.sh build [release|rls|debug|dbg]
# Debug模式
# ./script/build.sh build dbg
```

2. 运行

运行第1步编译的结果
```bash
./script/build.sh run
```

3. 清理缓存

清理第1步编译的缓存

```bash
./script/build.sh clean
```