//
// Created by swagger on 2022/2/27.
//


#include "ShardKvServer.h"
#include "ShardKv.h"
#include <iostream>

using std::cout;
using std::endl;
using std::cerr;



ShardKvServer::ShardKvServer(int port)
  : server_()
  , options_()
  , port_(port)
{

}

void ShardKvServer::start() {
  ShardKv s;
  if ( server_.AddService(&s, brpc::SERVER_DOESNT_OWN_SERVICE) != 0 ) {
    cerr << "Add server Fail" << endl;
    return;
  }

  options_.idle_timeout_sec = -1;
  if ( server_.Start(port_, &options_) != 0 ) {
    cerr << "Server Start at 7777 Failed" << endl;
    return;
  }
  cout << "Server Start at: " << port_ << " " << endl;
  server_.RunUntilAskedToQuit();
}

