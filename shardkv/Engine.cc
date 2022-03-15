//
// Created by swagger on 2022/2/27.
//

#include "Engine.h"
#include "TransactionContext.h"
#include "TransactionController.h"

#include <iostream>
using std::cout;
using std::endl;


std::mutex Engine::latch_ = {};
std::map<std::string, Tuple*> Engine::data_ = {};


static bool shared_lock(Tuple* t) {
  std::unique_lock<std::mutex> lock_guard(t->latch);
  if ( t->Tid == 0 ) {
    t->Rcnt += 1;
    return true;
  }
  return false;
}

static bool shared_lock_timeout(Tuple* t) {

}

static bool promote_shared_lock2exclusive_lock(TransactionContext* Tctx, Tuple* t) {
  std::unique_lock<std::mutex>  lock_guard(t->latch);
  if ( t->Tid == 0 && t->Rcnt == 1) {
    t->Tid = Tctx->id;
    t->Rcnt += 1;
    return true;
  }
  return false;
}

static void shared_unlock(Tuple* t) {
  std::unique_lock<std::mutex> lock_guard(t->latch);
  t->Rcnt -= 1;
}

static bool exclusive_lock(TransactionContext* Tctx, Tuple* t){
  std::unique_lock<std::mutex> lock_guard(t->latch);
//  cout << "----lock check----" << endl;
//  cout << "Tid: " << t->Tid << " " << "Rcnt: " << t->Rcnt << endl;
//  cout << "------------------" << endl;
  if ( t->Tid == 0 && t->Rcnt == 0 ) {
    t->Tid = Tctx->id;
    t->Rcnt += 1;
    return true;
  }
  return false;
}

static void exclusive_unlock(Tuple* t) {
  std::unique_lock<std::mutex> lock_guard(t->latch);
  t->Tid = 0;
  t->Rcnt = 0;
}

Tuple* Engine::getData(std::string& key) {
  std::unique_lock<std::mutex> lock_guard(latch_);
  auto iter = data_.find(key);
  if ( iter == data_.end() ){
    return nullptr;
  }
  return iter->second;
}

// false mean need use doUpdate, not doInsert
bool Engine::insert(std::string& key, Tuple *nOne) {
  std::unique_lock<std::mutex> lock_guard(latch_);
  auto iter = data_.find(key);
  if ( iter == data_.end()) {
    data_.insert(std::pair<std::string, Tuple*>(key, nOne));

    return true;
  }
  return false;
}



State Engine::read(TransactionContext* Tctx, std::string& key, std::string& value) {
  // check is read before?
  Tuple* ptr = nullptr;

  if ( (ptr = Tctx->writeCheck(key)) != nullptr ) {

    if ( ptr->next_.load() == nullptr ) { // insert by current transacton
      if ( ptr->invalid ) {
        return KeyNotExist; // !!
      }
      value = ptr->value;
      return Success; // !!
    }
    // read write before
    Tuple* newTuple = ptr->next_.load();
    if ( newTuple->invalid ) {
      return KeyNotExist; // !!
    }
    value = newTuple->value;
    return Success;
  }


  if ( (ptr = Tctx->readCheck(key)) != nullptr ) {
    if ( ptr->invalid ) {
      return KeyNotExist; // !!
    }
    value = ptr->value;
    return Success; // !!
  }


  // new read
  ptr = getData(key);
  if ( ptr == nullptr ) {
    return KeyNotExist; // !!
  }

  while ( ptr != nullptr ) {
    if ( ptr->BeginTS.load() <= Tctx->id && Tctx->id < ptr->EndTS.load() ) {
      break;
    }
    ptr = ptr->next_.load();
  }
  if ( ptr == nullptr ) {
    return KeyNotExist; // !!
  }

  // find read one and try to get lock
  // get shared lock fail
  if ( !shared_lock(ptr)) {
    return LockFailed; // !!
  }
  // TODO: record to ReadMap
  Tctx->ReadMap.insert(std::pair<std::string, Tuple*>(key, ptr));

  if ( ptr->invalid ) {
    return KeyNotExist; // !!
  }
  value = ptr->value;

  return Success; // !!
}


State Engine::write(TransactionContext* Tctx, std::string& key, std::string& value) {

  Tuple* ptr = nullptr;
  if ( ( ptr = Tctx->writeCheck(key)) != nullptr ) {
    if ( ptr->next_.load() == nullptr ) { // insert in current transaction
      ptr->value = value;
    }else {
      ptr->next_.load()->value = value;
    }
    return Success;
  }

  // read before, make shared_lock up to exclusive_lock
  if (Tctx->readCheck(key) != nullptr ) {
    return doUpdate(Tctx, nullptr, key, value);
  }


  ptr = getData(key);
  if (ptr == nullptr) {
    return doInsert(Tctx, key, value);
  } else {
    return doUpdate(Tctx, ptr, key, value);
  }

}

State Engine::doUpdate(TransactionContext* Tctx, Tuple* ptr, std::string& key, std::string& value) {

  // promote shared_lock to exclusive_lock
  if ( ptr == nullptr ) {
    ptr = Tctx->readCheck(key);
    if (!promote_shared_lock2exclusive_lock(Tctx, ptr)) {
      return LockFailed;
    }
    goto DO_INSERT;
  }


  while ( ptr != nullptr ) {
    if ( ptr->EndTS.load() == std::numeric_limits<uint64_t>::max()) {
      break;
    }
    ptr = ptr->next_.load();
  }

  // there has history record but delete
  if ( ptr == nullptr ){
    return doInsert(Tctx, key, value);
  }

  // get exclusive lock
  if (!exclusive_lock(Tctx, ptr)) {
    return LockFailed;
  }

  DO_INSERT:

//  cout << "lock: " << ptr->value << " ";
//  cout << "Tid(lock): " << ptr->Tid << " ";
//  cout << "Rcnt(ReadLock)" << ptr->Rcnt << endl;
  Tuple* newTuple = new Tuple(Tctx->id);
  newTuple->value = value;
  ptr->next_.store(newTuple);
  // TODO: record the write set
  // record current one, not new one
  Tctx->WriteMap.insert(std::pair<std::string, Tuple*>(key, ptr));
  return Success;
}

State Engine::doInsert(TransactionContext* Tctx, std::string& key, std::string& value) {
  Tuple* nOne = new Tuple(Tctx->id);
  exclusive_lock(Tctx, nOne); // lock before insert to data_;
  nOne->value = value;
  bool ret = insert(key, nOne);
  if ( !ret ) { // there has error, do it again
    delete nOne;
    return write(Tctx, key, value);
  }
  // TODO: record to write set
  Tctx->WriteMap.insert(std::pair<std::string, Tuple*>(key, nOne));
  //cout << "insert: " << key << " -> " << value << endl;
  return Success;
}

State Engine::doDel(TransactionContext* Tctx, Tuple* ptr, std::string& key) {
  while (ptr != nullptr) {
    if (ptr->EndTS.load() == std::numeric_limits<uint64_t>::max()) {
      break;
    }
    ptr = ptr->next_.load();
  }

  if (ptr == nullptr) {
    return KeyNotExist;
  }
  // get exclusive lock
  if (!exclusive_lock(Tctx, ptr)) {
    cout << "lock fail" << endl;
    return LockFailed;
  }
  Tuple *nTuple = new Tuple(Tctx->id);
  nTuple->invalid = true;
  // TODO: record the write set
  // record current one, not new one
  Tctx->WriteMap.insert(std::pair<std::string, Tuple *>(key, ptr));

  Tuple *old = nullptr;
  bool ret = ptr->next_.compare_exchange_strong(old, nTuple);
  if (!ret) {
    return Fail; // ?
  }
  return Success;
}

State Engine::commit(TransactionContext* Tctx) {
  for (auto old : Tctx->WriteMap) {
    Tuple* nOne = old.second->next_.load();
    if ( nOne == nullptr ) { // insert operation
      old.second->BeginTS.store(Tctx->commit_id);
    }else {
      bool ret = nOne->BeginTS.compare_exchange_strong(Tctx->id, Tctx->commit_id);
      if (!ret) {
        // TODO: FATAL! for debug
      }
      old.second->EndTS.store(Tctx->commit_id); // set old one
    }
    // unlock
    exclusive_unlock(old.second);
  }

  for (auto iter : Tctx->ReadMap ) {
    shared_unlock(iter.second);
  }
  return Success;
}

void Engine::abort(TransactionContext* Tctx) {
  for (auto old : Tctx->WriteMap ) {
    Tuple* nOne = old.second->next_.load();
    if ( nOne == nullptr ) { // insert need make invalid
      old.second->BeginTS.store(Tctx->commit_id);
      old.second->invalid = true;
    }else {
      old.second->next_.store(nullptr);
      delete nOne; // FIXME: safe? I think so
    }
    // unlock
    exclusive_unlock(old.second);
  }

  for (auto iter : Tctx->ReadMap) {
    shared_unlock(iter.second);
  }
}




// ******** for debug ***********



void Engine::show() {
  for (auto iter : data_ ) {
    std::string key = iter.first;
    Tuple* value = iter.second;
    cout << key << " -> " << value->value << " | ";
    cout << "invalid: " << value->invalid << " ";
    cout << "Begin: " << value->BeginTS.load() << " ";
    cout << "End: " << value->EndTS.load() << endl;
    Tuple* link = value->next_.load();
    while (link != nullptr) {
      cout << key << " -> " << link->value << " | ";
      cout << "invalid: " << link->invalid << " ";
      cout << "Begin: " << link->BeginTS.load() << " ";
      cout << "End: " << link->EndTS.load() << endl;
      link = link->next_.load();
    }
  }
}


