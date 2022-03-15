#include <iostream>
#include "../../shardkv/ShardKvClient.h"

using std::cout;
using std::endl;
using std::string;


using std::cin;

void Input(int tid) {
  ShardKvClient c("0.0.0.0:7777");
  cout << "start at tid: " << tid << endl;
  AGAIN:
  string op = "";
  cout << "your op: ";
  cin >> op;
  if ( op == "exit") {
    cout << State2Str(c.ABORT(tid)) << endl;
    return;
  }else if ( op ==  "read") {
      string key;
      string value;
      cout << "key: ";
      cin >> key;
      RpcState ret = c.PrepareRead(tid, key, value);
      cout << State2Str(ret) << endl;
      if ( ret == Prepare_OK )  {
        cout << "*******************" << endl;
        cout << key << " -> " << value << endl;
        cout << "*******************" << endl;
      }
      goto AGAIN;
  }else if ( op ==  "write") {
      std::string key;
      std::string value;
      cout << "key: ";
      cin >> key;
      cout << "value: ";
      cin >> value;
      cout << State2Str(c.PrepareWrite(tid, key, value)) << endl;
      goto AGAIN;
  }else if (op == "begin") {
      cout << State2Str(c.BEGIN(tid)) << endl;
      goto AGAIN;
  }else if (op ==  "commit") {
    int commitID = 0;
    cout << "commit id: ";
    cin >> commitID;
    cout << State2Str(c.END(tid, commitID)) << endl;
    return;
  } else if ( op ==  "abort") {
      cout << State2Str(c.ABORT(tid)) << endl;
      return;
  }else {
      cout << "FAIL op" << endl;
      cout << State2Str(c.ABORT(tid)) << endl;
      return;
  }
}


int main(int argc, char** args) {


  if ( argc < 2 ) {
    cout << "ShardKvClient + number" << endl;
    return -1;
  }

  Input(atoi(args[1]));
//  if ( atoi(args[1]) == 1) {
//    add();
//  }else if (atoi(args[1]) == 2 ) {
//    query();
//  }else if (atoi(args[1]) == 3) {
//    abortTest();
//  } else if ( atoi(args[1]) == 4) {
//    handleAbort();
//  }
}