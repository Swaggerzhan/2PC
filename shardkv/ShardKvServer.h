//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_SHARDKVSERVER_H
#define INC_2PC_SHARDKVSERVER_H

#include "brpc/server.h"


class ShardKvServer {
public:

  ShardKvServer(int);
  ~ShardKvServer()=default;

  void start();


private:

  brpc::Server server_;
  brpc::ServerOptions options_;

  int port_;

};






#endif //INC_2PC_SHARDKVSERVER_H
