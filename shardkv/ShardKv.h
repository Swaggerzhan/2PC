//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_SHARDKV_H
#define INC_2PC_SHARDKV_H


#include "../proto/shardkv.pb.h"

struct TransactionContext;

typedef std::pair<int, TransactionContext*> TMapRecord;


class ShardKv : public ShardKvBase {
public:
  void BEGIN(::google::protobuf::RpcController*, const ::ShardBeginArgs*, ::ShardBeginReply*,
             ::google::protobuf::Closure*) override;


  void END(::google::protobuf::RpcController*, const ::ShardEndArgs*, ::ShardEndReply*,
           ::google::protobuf::Closure*) override;

  void PrepareRead(::google::protobuf::RpcController*,
           const ::ShardReadArgs*, ::ShardReadReply*, ::google::protobuf::Closure*) override;

  void PrepareWrite(::google::protobuf::RpcController*,
           const ::ShardWriteArgs*, ::ShardWriteReply*, ::google::protobuf::Closure*) override;

  // TODO:ABORT
  void ABORT(::google::protobuf::RpcController*,
             const ::ShardAbortArgs*, ::ShardAbortReply*, ::google::protobuf::Closure*) override;

private:

  void removeContext(int);

private:
  std::map<int, TransactionContext*> Tmap_;
  std::mutex mutex_;

};




#endif //INC_2PC_SHARDKV_H
