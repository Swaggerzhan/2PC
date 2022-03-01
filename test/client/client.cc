#include <iostream>
#include "../../shardkv/ShardKvClient.h"

using std::cout;
using std::endl;
using std::string;


void add() {
  ShardKvClient c("0.0.0.0:7777");

  string key = "Swagger";
  string key2 = "Bob";
  string key3 = "Alice";
  string value = "100000";
  string value2 = "Bob has not money";
  string value3 = "Alice say fuck you!";

  int tid = 1;

  cout << c.BEGIN(tid) << endl;
  cout << c.PrepareWrite(tid, key, value) << endl;
  cout << c.PrepareWrite(tid, key2, value2) << endl;
  cout << c.PrepareWrite(tid, key3, value3) << endl;
  cout << c.END(tid) << endl;
}

void query() {

  ShardKvClient c("0.0.0.0:7777");

  string key = "Swagger";
  string key2 = "Bob";
  string key3 = "Alice";
  string value;
  string value2;
  string value3;
  int tid = 2;

  cout << c.BEGIN(tid) << endl;
  cout << c.PrepareRead(tid, key, value) << endl;
  cout << c.PrepareRead(tid, key2, value2) << endl;
  cout << c.PrepareRead(tid, key3, value3) << endl;
  cout << c.END(tid) << endl;

  cout << value << endl;
  cout << value2 << endl;
  cout << value3 << endl;

}


void abortTest() {
  ShardKvClient c("0.0.0.0:7777");

  string key = "Swagger";
  string key2 = "Bob";
  string key3 = "Alice";
  string key4 = "NotExist";
  string value = "100000";
  string value2 = "Bob has not money";
  string value3 = "Alice say fuck you!";
  string value4 = "";

  int tid = 1;

  cout << c.BEGIN(tid) << endl;
  cout << c.PrepareWrite(tid, key, value) << endl;
  cout << c.PrepareWrite(tid, key2, value2) << endl;
  cout << c.PrepareWrite(tid, key3, value3) << endl;
  cout << c.PrepareRead(tid, key4, value4) << endl;
  cout << c.END(tid) << endl;
}


int main(int argc, char** args) {
  if ( argc < 2 ) {
    cout << "ShardKvClient + number" << endl;
    return -1;
  }
  if ( atoi(args[1]) == 1) {
    add();
  }else if (atoi(args[1]) == 2 ) {
    query();
  }else if (atoi(args[1]) == 3) {
    abortTest();
  }
}