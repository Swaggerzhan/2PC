#include "../../coordinator/CoordinatorClient.h"
#include "../../util/global.h"
#include <iostream>

using std::string;
using std::cout;
using std::endl;

void add() {
  CoordinatorClient c("0.0.0.0:8888");

  string key = "Swagger";
  string key1 = "Bob";
  string key2 = "Alice";
  string value = "100000";
  string value1 = "Bob has no money";
  string value2 = "Alice say fuck you!";



  cout << State2Str(c.BEGIN()) << endl;
  cout << State2Str(c.Write(key, value)) << endl;
  cout << State2Str(c.Write(key1, value1)) << endl;
  cout << State2Str(c.Write(key2, value2)) << endl;
  cout << State2Str(c.END()) << endl;

}


void add2() {
  CoordinatorClient c("0.0.0.0:8888");

  string key = "Swagger";
  string key1 = "Bob";
  string key2 = "Alice";
  string key3 = "xxxxxx";
  string key4 = "yyyyyyy";
  string key5 = "zzzzzzzz";
  string key6 = "aaaaaaaa";
  string key7 = "ttttttttt";
  string key8 = "cccccccccc";

  string value = "100000";
  string value1 = "Bob has no money";
  string value2 = "Alice say fuck you!";



  cout << State2Str(c.BEGIN()) << endl;
  cout << State2Str(c.Write(key, value)) << endl;
  cout << State2Str(c.Write(key1, value1)) << endl;
  cout << State2Str(c.Write(key2, value2)) << endl;
  cout << State2Str(c.Write(key3, key3)) << endl;
  cout << State2Str(c.Write(key4, key4)) << endl;
  cout << State2Str(c.Write(key5, key5)) << endl;
  cout << State2Str(c.Write(key6, key6)) << endl;
  cout << State2Str(c.Write(key7, key7)) << endl;
  cout << State2Str(c.Write(key8, key8)) << endl;
  cout << State2Str(c.END()) << endl;
}


void query() {
  CoordinatorClient c("0.0.0.0:8888");

  string key = "Swagger";
  string key1 = "Bob";
  string key2 = "Alice";
  string value;
  string value1;
  string value2;



  cout << State2Str(c.BEGIN()) << endl;
  cout << State2Str(c.Read(key, value)) << endl;
  cout << State2Str(c.Read(key1, value1)) << endl;
  cout << State2Str(c.Read(key2, value2)) << endl;
  cout << State2Str(c.END()) << endl;

  cout << value << endl;
  cout << value1 << endl;
  cout << value2 << endl;

}

void query2() {
  CoordinatorClient c("0.0.0.0:8888");

  string key3 = "xxxxxx";
  string key4 = "yyyyyyy";
  string key5 = "zzzzzzzz";
  string key6 = "aaaaaaaa";
  string key7 = "ttttttttt";
  string key8 = "cccccccccc";

  string value3;
  string value4;
  string value5;
  string value6;
  string value7;
  string value8;



  cout << State2Str(c.BEGIN()) << endl;
  cout << State2Str(c.Read(key3, value3)) << endl;
  cout << State2Str(c.Read(key4, value4)) << endl;
  cout << State2Str(c.Read(key5, value5)) << endl;
  cout << State2Str(c.Read(key6, value6)) << endl;
  cout << State2Str(c.Read(key7, value7)) << endl;
  cout << State2Str(c.Read(key8, value8)) << endl;
  cout << State2Str(c.END()) << endl;

  cout << value3 << endl;
  cout << value4 << endl;
  cout << value5 << endl;
  cout << value6 << endl;
  cout << value7 << endl;
  cout << value8 << endl;



}



int main(int argc, char** args) {

  if ( argc < 2 ) {
    cout << "coordinatorClient + number" << endl;
    cout << "0 for add2" << endl;
    cout << "1 for add " << endl;
    cout << "2 for query" << endl;
    cout << "3 for query2" << endl;
    return -1;
  }
  int x = atoi(args[1]);

  if ( x == 1 ) {
    add();
  }else if (x == 2 ) {
    query();
  }else if (x == 0 ) {
    add2();
  }else if (x == 3) {
    query2();
  }


}