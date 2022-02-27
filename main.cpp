#include <iostream>
#include "bthread/types.h"
#include "shardkv/Engine.h"
#include "shardkv/TransactionContext.h"
#include "shardkv/KvServer.h"

using std::string;
using std::cout;
using std::endl;

int main() {


  TransactionContext* t = new TransactionContext;

  Engine::BEGIN(t);

  string key = "Swagger";
  string value = "10000";
  string key1 = "Bob";
  string value1 = "100";
  Engine::write(key, value);
  Engine::write(key1, value1);
  Engine::END();
  Engine::show();

}
