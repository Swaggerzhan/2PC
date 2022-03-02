//
// Created by swagger on 2022/3/2.
//
#include "CoordinatorServer.h"
#include "TransactionCoordinator.h"

using std::cout;
using std::endl;
using std::cerr;
using std::string;



CoordinatorServer::CoordinatorServer(int port)
        : server_()
        , options_()
        , port_(port)
{
}

void CoordinatorServer::start() {
  std::vector<string> hosts = {"0.0.0.0:6666", "0.0.0.0:7777"};
  TransactionCoordinator coor(hosts);
  if ( server_.AddService(&coor, brpc::SERVER_DOESNT_OWN_SERVICE) != 0 ) {
    cerr << "Add server Fail" << endl;
    return;
  }

  options_.idle_timeout_sec = -1;
  if ( server_.Start(port_, &options_) != 0 ) {
    cerr << "Server Start at 7777 Failed" << endl;
    return;
  }
  cout << "CoordinatorServer Start at: " << port_ << " " << endl;
  server_.RunUntilAskedToQuit();
}