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

struct Entry {
  std::string value;
};

enum State {
  Success,
  Fail,
  KeyNotExist,
};


class Engine{
public:

  static void BEGIN(TransactionContext*);
  static void END(int);
  static void ABORT(int);

  // don't call it!
  static void COMMIT();

  static void ROLLBACK(int);

  // key -> value
  static State read(int, std::string&, std::string&);
  static State write(int, std::string&, std::string&);
  static State del(int, std::string&);

  // for debug
  static void show();

private:

  static std::mutex mutex_;
  static std::map<std::string, std::shared_ptr<Entry>> data_;
  static TransactionContext* lockBy_;

};

#endif //INC_2PC_ENGINE_H
