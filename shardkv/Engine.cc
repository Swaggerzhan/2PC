//
// Created by swagger on 2022/2/27.
//

#include "Engine.h"
#include "TransactionContext.h"
#include "TransactionController.h"


#include <iostream>
using std::cout;
using std::endl;

std::mutex Engine::mutex_ = {};
std::map<std::string, std::shared_ptr<Entry>> Engine::data_ = {};
TransactionContext* Engine::lockBy_ = nullptr;


Entry::Entry()
: invalid(false)
{}


static void copyEntry(std::shared_ptr<Entry>& dst, std::shared_ptr<Entry>& srt) {
  dst->value = srt->value;
  dst->invalid = srt->invalid;
}


// 开始
void Engine::BEGIN(TransactionContextPtr& Tctx) {

}

// 结束
bool Engine::END(TransactionContextPtr& Tctx) {
  if (Tctx->invalid) {
    cout << "End by Error" << endl;
    show();
    return false;
  }
  COMMIT(Tctx);
  cout << "End by OK" << endl;
  show();
  return true;
}

// COMMIT， 直接解锁即可 xxxx
void Engine::COMMIT(TransactionContextPtr& Tctx) {

  while ( !Tctx->lockTable.empty() ) {
    auto t = Tctx->lockTable.back();
    Tctx->lockTable.pop_back();
    t->latch.unlock();
  }

}

void Engine::ABORT(TransactionContextPtr& Tctx) {
  ROLLBACK(Tctx);
  Tctx->invalid = true;
}



State Engine::read(TransactionContextPtr& Tctx, std::string& key, std::string& ret) {
  std::shared_ptr<Entry> entry;
  if (!getData(key, entry)) {
    return KeyNotExist;
  }
  bool r = entry->latch.try_lock_for(std::chrono::seconds(TimeoutGenerator()));
  if (!r) {
    return LockFailed;
  }
  Tctx->lockTable.push_back(entry);
  if ( entry->invalid ) { // 删除位
    return KeyNotExist;
  }
  ret = entry->value;
  return Success;
}

State Engine::write(TransactionContextPtr& Tctx, std::string& key, std::string& value) {
  mutex_.lock();
  // 需要先检测是否拥有新值
  auto iter = data_.find(key);
  if ( iter == data_.end()) {
    std::shared_ptr<Entry> entry(new Entry);
    entry->value = value;
    data_.insert(Record(key, entry));
    entry->latch.lock();
    Tctx->lockTable.push_back(entry); // 锁表
    Tctx->undoList.emplace_back(UndoRecord(Delete, Record(key, entry))); // undoList
    mutex_.unlock();
    return Success;
  }
  // 更新值
  std::shared_ptr<Entry> entry = iter->second;
  mutex_.unlock();
  bool r = entry->latch.try_lock_for(std::chrono::seconds(TimeoutGenerator()));
  if (!r) {
    return LockFailed;
  }
  Tctx->lockTable.push_back(entry);
  // undoList
  std::shared_ptr<Entry> back(new Entry);
  copyEntry(back, entry);
  Tctx->undoList.emplace_back(UndoRecord(Replace, Record(key, back)));
  entry->value = value;
  entry->invalid = false;
  return Success;
}

State Engine::del(TransactionContextPtr& Tctx, std::string& key) {

  std::shared_ptr<Entry> entry;
  if (!getData(key, entry)) {
    return KeyNotExist;
  }
  bool r = entry->latch.try_lock_for(std::chrono::seconds(TimeoutGenerator()));
  if (!r) {
    return LockFailed;
  }
  Tctx->lockTable.push_back(entry);
  entry->invalid = true; // 标记为删除
  std::shared_ptr<Entry> back;
  copyEntry(back, entry);
  Tctx->undoList.emplace_back(UndoRecord(Replace, Record(key, back)));
  return Success;
}

void Engine::ROLLBACK(TransactionContextPtr& Tctx) {

  while (!Tctx->undoList.empty()) {
    auto u = Tctx->undoList.back();
    Tctx->undoList.pop_back();
    if (u.first == Delete) {
      u.second.second->invalid = true; // 标记为删除
    }else if (u.first == Replace){
      mutex_.lock();
      auto iter = data_.find(u.second.first);
      // 复原
      iter->second->invalid = u.second.second->invalid;
      iter->second->value = u.second.second->value;
      mutex_.unlock();
    }
  }

  while (!Tctx->lockTable.empty()) {
    auto t = Tctx->lockTable.back();
    Tctx->lockTable.pop_back();
    t->latch.unlock();
  }
}



bool Engine::getData(std::string& key, std::shared_ptr<Entry>& entry) {
  std::unique_lock<std::mutex> lockGuard(mutex_);
  auto iter = data_.find(key);
  if (iter == data_.end()) {
    return false;
  }
  entry = iter->second;
  return true;
}


void Engine::pushUndoRecord(TransactionContext* Tctx, std::string& key, std::shared_ptr<Entry> entry) {
  std::shared_ptr<Entry> back;
  copyEntry(entry, back);
}





// ******** for debug ***********

#include <iostream>
using std::cout;
using std::endl;


void Engine::show() {
  // don't lock, just for debug
  cout << "******** SHOW BEGIN ********" << endl;
  for (auto& iter : data_) {
    if (iter.second->invalid) {
      continue;
    }
    cout << iter.first << " -> ";
    cout << iter.second->value << endl;
  }
  cout << "******** SHOW END ***********" << endl;
}



