//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_SERVER_H
#define INC_2PC_SERVER_H

#include "brpc/server.h"

class Server {
public:

  Server();

  ~Server()=default;

  void start();


private:

 brpc::Server server_;
 brpc::ServerOptions options_;

};




#endif //INC_2PC_SERVER_H
