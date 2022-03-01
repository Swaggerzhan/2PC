#include <iostream>
#include "../../shardkv/ShardKvServer.h"


int main() {


  ShardKvServer s(7777);
  s.start();

}


