//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_TRANSACTIONCONTROLLER_H
#define INC_2PC_TRANSACTIONCONTROLLER_H


#include <atomic>
#include <memory>
#include <vector>

struct Entry;

class TransactionID {
public:

  static int get();

private:
  static std::atomic<int> id_;
};


int TimeoutGenerator();


class TransactionController {
public:


private:
  int Tid_;
  std::vector<std::shared_ptr<Entry>> lockTable_;
};









#endif //INC_2PC_TRANSACTIONCONTROLLER_H
