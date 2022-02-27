//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_KVSERVER_H
#define INC_2PC_KVSERVER_H


#include "../proto/KvServer.pb.h"
#include <map>
#include <mutex>
#include "../rpc/global.h"

struct TransactionContext;

typedef std::pair<int, TransactionContext*> TMapRecord;





class KvServer : public KvServerBase {
public:

  void BEGIN(::google::protobuf::RpcController*, const ::BeginArgs*, ::BeginReply*,
             ::google::protobuf::Closure*) override;


  void END(::google::protobuf::RpcController*, const ::EndArgs*, ::EndReply*,
           ::google::protobuf::Closure*) override;

  void Get(::google::protobuf::RpcController*,
           const ::GetArgs*, ::GetReply*, ::google::protobuf::Closure*) override;

  void Put(::google::protobuf::RpcController*,
           const ::PutArgs*, ::PutReply*, ::google::protobuf::Closure*) override;

private:

  std::map<int, TransactionContext*> Tmap_;
  std::mutex mutex_;

};







#endif //INC_2PC_KVSERVER_H
