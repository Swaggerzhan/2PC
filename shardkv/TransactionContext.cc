//
// Created by swagger on 2022/2/27.
//


#include "TransactionContext.h"
#include "TransactionController.h"


Tuple::Tuple(uint64_t begin)
: latch()
, Tid(0)
, Rcnt(0)
, BeginTS(begin)
, EndTS(std::numeric_limits<uint64_t>::max())
, value("")
, next_(nullptr)
, invalid(false)
{}


Tuple *TransactionContext::writeCheck(std::string& key) {
  auto iter = WriteMap.find(key);
  if ( iter == WriteMap.end() ) {
    return nullptr;
  }
  return iter->second;
}

Tuple* TransactionContext::readCheck(std::string& key) {
  auto iter = ReadMap.find(key);
  if ( iter == ReadMap.end() ) {
    return nullptr;
  }
  return iter->second;
}

TransactionContext::TransactionContext()
: id(-1)
, commit_id(-1)
, invalid(false)
, ReadMap()
, WriteMap()
{}

