#include <iostream>
#include "../../shardkv/ShardKvServer.h"

using std::cout;
using std::endl;


int main(int argc, char** args) {

  if ( argc <  2) {
    cout << "ShardKvServer + port"  << endl;
    return -1;
  }


  ShardKvServer s(atoi(args[1]));
  s.start();

}


