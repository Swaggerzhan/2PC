//
// Created by swagger on 2022/3/2.
//

#ifndef INC_2PC_COORDINATORSERVER_H
#define INC_2PC_COORDINATORSERVER_H

#include "brpc/server.h"


class CoordinatorServer {
public:

  CoordinatorServer(int);
  ~CoordinatorServer()=default;

  void start();


private:
  brpc::Server server_;
  brpc::ServerOptions options_;

  int port_;

};

#endif //INC_2PC_COORDINATORSERVER_H
