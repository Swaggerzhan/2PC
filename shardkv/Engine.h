//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_ENGINE_H
#define INC_2PC_ENGINE_H


#include <mutex>
#include <thread>
#include <map>
#include <memory>

struct TransactionContext;

typedef std::shared_ptr<TransactionContext> TransactionContextPtr;

struct Entry {

  Entry();

  std::timed_mutex latch;
  std::string value;
  bool invalid;
};

enum State {
  Success,
  Fail,
  KeyNotExist,
  LockFailed,
};


class Engine{
public:

  static void BEGIN(TransactionContextPtr&);
  static bool END(TransactionContextPtr&);
  static void ABORT(TransactionContextPtr&);

  // don't call it!
  static void COMMIT(TransactionContextPtr&);

  static void ROLLBACK(TransactionContextPtr&);

  // key -> value
  static State read(TransactionContextPtr&, std::string&, std::string&);
  static State write(TransactionContextPtr&, std::string&, std::string&);
  static State del(TransactionContextPtr&, std::string&);

  // for debug
  static void show();

private:

  static void pushUndoRecord(TransactionContext*, std::string&, std::shared_ptr<Entry>);



  // 从data_中获取数据
  static bool getData(std::string&, std::shared_ptr<Entry>&);
  static bool writeData(std::string&, std::shared_ptr<Entry>&);

  // 被删除的Entry需要提前上锁
  static bool delData(std::string&);


private:

  static std::mutex mutex_;
  static std::map<std::string, std::shared_ptr<Entry>> data_;
  static TransactionContext* lockBy_;

};

#endif //INC_2PC_ENGINE_H
