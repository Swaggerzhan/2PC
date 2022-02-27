//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_KVCLIENT_H
#define INC_2PC_KVCLIENT_H


#include "../proto/KvServer.pb.h"
#include "brpc/channel.h"
#include "gflags/gflags.h"


class KvClient {
public:

  // 0.0.0.0:8888
  KvClient(std::string);

  ~KvClient()=default;

  bool BEGIN();
  bool read(std::string&, std::string&);
  bool write(std::string&, std::string&);
  bool END();


private:
  brpc::Channel channel_;
  brpc::ChannelOptions options_;
  KvServerBase_Stub* stub_;
  int Tid_;

};






#endif //INC_2PC_KVCLIENT_H
