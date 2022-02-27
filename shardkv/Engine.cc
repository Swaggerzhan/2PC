//
// Created by swagger on 2022/2/27.
//

#include "Engine.h"
#include "TransactionContext.h"

std::mutex Engine::mutex_ = {};
std::map<std::string, std::shared_ptr<Entry>> Engine::data_ = {};
TransactionContext* Engine::lockBy_ = nullptr;


static void copyEntry(std::shared_ptr<Entry>& dst, std::shared_ptr<Entry>& srt) {
  dst->value = srt->value;
}


// 开始
void Engine::BEGIN(TransactionContext *Tctx) {
  mutex_.lock();
  lockBy_ = Tctx;
}

// 结束
void Engine::END(int id) {
  if ( lockBy_ == nullptr || lockBy_->t_id != id ) {
    return;
  }
  COMMIT();
}

// COMMIT， 直接解锁即可 xxxx
void Engine::COMMIT() {
  lockBy_ = nullptr;
  mutex_.unlock();
}

void Engine::ABORT(int id) {
  if ( lockBy_ == nullptr || lockBy_->t_id != id ) {
    return;
  }
  ROLLBACK(id);
  lockBy_ = nullptr;
  mutex_.unlock();
}



State Engine::read(int id, std::string& key, std::string& ret) {
  if ( lockBy_ == nullptr || lockBy_->t_id != id ) {
    return Fail;
  }

  auto iter = data_.find(key);
  if (iter == data_.end()) {
    return KeyNotExist;
  }
  ret = iter->second->value;
  return Success;
}

State Engine::write(int id, std::string& key, std::string& value) {
  if ( lockBy_ == nullptr || lockBy_->t_id != id )  {
    return Fail;
  }

  auto iter = data_.find(key);
  if (iter == data_.end()) { // new value
    Record record(key, std::shared_ptr<Entry>(new Entry));
    record.second->value = value;
    data_.insert(record);
    lockBy_->undoList.emplace_back(UndoRecord(Delete, record)); // for undo
    return Success;
  }
  std::shared_ptr<Entry> back(new Entry);
  copyEntry(back, iter->second); // 备份
  iter->second->value = value; // 写入
  Record record(key, back);
  lockBy_->undoList.emplace_back(Replace, record);
  return Success;
}

State Engine::del(int id, std::string& key) {
  if ( lockBy_ == nullptr || lockBy_->t_id != id ) {
    return Fail;
  }

  auto iter = data_.find(key);
  if ( iter == data_.end() ) {
    return KeyNotExist;
  }

  Record record(key, iter->second);
  lockBy_->undoList.emplace_back(UndoRecord(Insert, record));
  data_.erase(iter);
  return Success;
}

void Engine::ROLLBACK(int id) {
  if ( lockBy_ == nullptr || lockBy_->t_id != id) {
    // TODO: fatal
  }
  while ( !lockBy_->undoList.empty() ) {
    UndoRecord r = lockBy_->undoList.back();
    lockBy_->undoList.pop_back();
    if ( r.first == Delete ) {
      data_.erase(r.second.first);
    }else if ( r.first == Replace ) {
      data_.insert(r.second);
    }else if ( r.first == Insert ) {
      data_.insert(r.second);
    }
  }
}

#include <iostream>
using std::cout;
using std::endl;


void Engine::show() {
  // don't lock, just for debug
  cout << "******** SHOW BEGIN ********" << endl;
  for (auto& iter : data_) {
    cout << iter.first << " -> ";
    cout << iter.second->value << endl;
  }
  cout << "******** SHOW END ***********" << endl;
}



