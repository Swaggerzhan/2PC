//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_TRANSACTIONCOORDINATOR_H
#define INC_2PC_TRANSACTIONCOORDINATOR_H


#include "../proto/transaction.pb.h"
#include "../shardkv/ShardKvClient.h"
#include <map>
#include <mutex>
#include <memory>

typedef std::shared_ptr<ShardKvClient> ShardKvClientPtr;
typedef std::pair<int, std::vector<ShardKvClientPtr>*> TransactionRecord;


class TransactionCoordinator : public TransactionCoordinatorBase {
public:

  TransactionCoordinator();
  ~TransactionCoordinator()=default;

  void BEGIN(::google::protobuf::RpcController*,
             const ::BeginArgs*,
             ::BeginReply*,
             ::google::protobuf::Closure*) override;

  void END(::google::protobuf::RpcController*,
           const ::EndArgs*,
           ::EndReply*,
           ::google::protobuf::Closure*)override;


  void READ(::google::protobuf::RpcController*,
            const ::ReadArgs*,
            ::ReadReply*,
            ::google::protobuf::Closure*)override;

  void WRITE(::google::protobuf::RpcController*,
             const ::WriteArgs*,
             ::WriteReply*,
             ::google::protobuf::Closure*) override;

private:

  void errorAndAbort(int, std::vector<ShardKvClientPtr>*);

private:

  // 键通过hash后所对应的存储服务器
  std::map<int, ShardKvClientPtr> shardKv_;
  std::mutex mutex_;

  // 每个事务ID和其对应的使用了多少个ShardKV
  std::map<int, std::vector<ShardKvClientPtr>*> transactions_;
  std::mutex latch_;

  const int kRound_;
  const int kSeed_;


};





#endif //INC_2PC_TRANSACTIONCOORDINATOR_H
