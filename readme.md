# 一个 Anns 相关算法学习库

随机更新一些学了的算法的简单 c++ 实现。

目前已实现 ：

+ hnsw 算法



## hnsw

### 简述

代码实现均在 `hnsw/hnsw.h` 中， 调用直接 `#include "../hnsw/hnsw.h"` 即可。

类原型 `template<typename T = int> hnsw` ， 默认 `int`， 可传入参数， 修改向量元素类型。

默认参数 `efConstruction = 100, M = 50, Mmax = M, Mmax0 = M * 2, Ml = 1 / log(M)，alpha = 0.3`，其中 `alpha` 是为了实现简单的懒惰删除构造的重构因子。

 ### 函数说明

+ `hnsw()` 默认构造函数。
+ `hnsw(int len)` 预留 `len` 个向量的构造函数， 减少空间构造开销。
+ `bool erase(const ull id)` 删除编号为 `id` 的向量。
+ `vector<ull> k_nn_search_cookie(const vector<T> &q, int K, int ef = efConstruction)` 
    + 找到前 `K` 接近向量的元素 `id`。
+ `vector<double> get_min_kth_dist(const vector<T> &q, int K)` 返回向量点积。
+ `void save_data(string FILE_NAME)` 将当前数据存入文件 `FILE_NAME` ， 减小重复调用开销。
+ `void read_data(string FILE_NAME)` 将文件`FILE_NAME`内容读取到程序。
+ `ull insert(const vector<T> &q, ull Cookie = 0)` 插入向量， 向量对应的 `id` 是 `Cookie`， 默认为空， 可以任意指定， 但不应当重复。且在使用的时候应当全程自己制定或者交给库自动指定， 也就是不修改这个参数。



## test

test 部分在随机数据下与朴素循环算法进行正确性比对。应当注意比对应使用 `int` 类型。