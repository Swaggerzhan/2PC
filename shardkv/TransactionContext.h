//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_TRANSACTIONCONTEXT_H
#define INC_2PC_TRANSACTIONCONTEXT_H


#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <map>

struct Tuple {

  Tuple(uint64_t);

  std::mutex latch; // for struct, protect for Tid and Rcnt
  uint64_t Tid;
  uint32_t Rcnt;

  std::atomic<uint64_t> BeginTS;
  std::atomic<uint64_t> EndTS;

  // for value
  std::string value;

  bool invalid;

  std::atomic<Tuple*> next_;

};

struct TransactionContext {

  TransactionContext();

  uint64_t id;
  uint64_t commit_id;

  Tuple* writeCheck(std::string&);
  Tuple* readCheck(std::string&);

  std::map<std::string, Tuple*> ReadMap;
  std::map<std::string, Tuple*> WriteMap;

  // mark this bit when Transaction failed
  bool invalid;

};




#endif //INC_2PC_TRANSACTIONCONTEXT_H
