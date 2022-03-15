<font face="Monaco">

# 分布式事务2PC的C++简单实现(纯内存KV数据库)

```
requirement
    Linux kernel >= 2.6
    brpc
    protobuf >= 3.19.3
    C++ >= 11
```

for practice, 不考虑持久化。

### ShardKvServer

分片Server可以独自运行，作为一台完整的数据库，其中使用MV2PL的事务控制器，采用append only，O2N的链表进行存储(GC还未实现)。当事务控制器检测到LockFailed后会立刻将该事务mark为错误的，直到等待ABORT信号的到来(Coordinator)。

### TransactionCoordinator

分布式事务协调器，控制着事务的过程，一旦Coordiator发现一个参与的ShardKvServer PrepareFailed后，会立刻回滚其他参与者上的事务。

[MV2PL](https://github.com/Swaggerzhan/linux_md/blob/master/database/mvcc.md)

</font>
