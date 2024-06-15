#! /bin/bash

# 清楚缓存
./build.sh clean

# 编译发布
./build.sh release

# 打标签
sgit tag v1.0.1
git push origin v1.0.1
git push github v1.0.1

# 参考文档
# https://blog.csdn.net/qq_39866016/article/details/108773144
