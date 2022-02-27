//
// Created by swagger on 2022/2/27.
//

#include "Server.h"
#include "../shardkv/KvServer.h"
#include <iostream>
#include "gflags/gflags.h"

using std::cerr;
using std::cout;
using std::endl;

Server::Server()
: server_()
, options_()
{
}

void Server::start() {
  KvServer s;
  if ( server_.AddService(&s, brpc::SERVER_DOESNT_OWN_SERVICE) != 0 ) {
    cerr << "Add server Fail" << endl;
    return;
  }

  options_.idle_timeout_sec = -1;
  if ( server_.Start(7777, &options_) != 0 ) {
    cerr << "Server Start at 7777 Failed" << endl;
    return;
  }
  server_.RunUntilAskedToQuit();
}



