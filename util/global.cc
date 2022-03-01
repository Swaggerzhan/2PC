//
// Created by swagger on 2022/2/27.
//

#include "global.h"


std::string State2Str(RpcState s) {
  switch (s) {
    case Prepare_Failed: {
      return "PrepareFailed";
    }
    case Prepare_NotInit: {
      return "PrepareNotInit";
    }
    case Prepare_ConnectFail: {
      return "PrepareNetworkFailed";
    }
    case Prepare_OK: {
      return "OK";
    }
    default: {
      return "State Not Found";
    }
  }
}



