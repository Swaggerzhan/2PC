<font face="Monaco">

# 分布式事务2PC的C++简单实现

```
requirement
    Linux kernel >= 2.6
    brpc
    protobuf >= 3.19.3
    C++ >= 11
```

for practice

### ShardKvServer

分片Server仅以单机运行时，仅有2PL生效，当有多台ShardKvServer存在时，可以通过Coordinator来协调事务，分布式事务使用2PC。

### TransactionCoordinator

事务协调器。


</font>
