#include "../../client/KvClient.h"
#include <iostream>

using std::cout;
using std::endl;


int main() {

  KvClient c("0.0.0.0:7777");

  std::string key1 = "Swagger";
  std::string key2 = "Bob";
  std::string key3 = "Alice";
  std::string ret;


  cout << c.BEGIN() << endl;

  c.read(key1, ret);
  cout << ret << endl;
  c.read(key2, ret);
  cout << ret << endl;
  c.read(key3, ret);
  cout << ret << endl;

  cout << c.END() << endl;


}