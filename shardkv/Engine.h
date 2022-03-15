//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_ENGINE_H
#define INC_2PC_ENGINE_H


#include <mutex>
#include <thread>
#include <map>
#include <memory>
#include <atomic>

struct TransactionContext;


struct Tuple;



enum State {
  Success,
  Fail,
  KeyNotExist,
  LockFailed,
};


class Engine{
public:



  static State commit(TransactionContext*);
  static State read(TransactionContext*, std::string&, std::string&);
  static State write(TransactionContext*, std::string&, std::string&);
  static void abort(TransactionContext*);



  // for debug
  static void show();

private:


  static State doUpdate(TransactionContext*, Tuple*, std::string&, std::string&);
  static State doInsert(TransactionContext*, std::string&, std::string&);
  static State doDel(TransactionContext*, Tuple*, std::string&);



  /////// for struct data_
  static Tuple* getData(std::string&);
  static bool insert(std::string&, Tuple*);



private:


  static std::mutex latch_;
  static std::map<std::string, Tuple*> data_;

};

#endif //INC_2PC_ENGINE_H
