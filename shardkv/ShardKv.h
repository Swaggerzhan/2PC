//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_SHARDKV_H
#define INC_2PC_SHARDKV_H


#include "../proto/shardkv.pb.h"

struct TransactionContext;

typedef std::pair<int, std::shared_ptr<TransactionContext>> TMapRecord;


class ShardKv : public ShardKvBase {
public:
  void BEGIN(::google::protobuf::RpcController*, const ::BeginArgs*, ::BeginReply*,
             ::google::protobuf::Closure*) override;


  void END(::google::protobuf::RpcController*, const ::EndArgs*, ::EndReply*,
           ::google::protobuf::Closure*) override;

  void PrepareRead(::google::protobuf::RpcController*,
           const ::ReadArgs*, ::ReadReply*, ::google::protobuf::Closure*) override;

  void PrepareWrite(::google::protobuf::RpcController*,
           const ::WriteArgs*, ::WriteReply*, ::google::protobuf::Closure*) override;

  // TODO:ABORT
  void ABORT(::google::protobuf::RpcController*,
             const ::AbortArgs*, ::AbortReply*, ::google::protobuf::Closure*) override;

private:

  void removeContext(int);

private:
  std::map<int, std::shared_ptr<TransactionContext>> Tmap_;
  std::mutex mutex_;

};




#endif //INC_2PC_SHARDKV_H
