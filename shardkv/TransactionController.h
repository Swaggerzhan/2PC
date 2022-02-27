//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_TRANSACTIONCONTROLLER_H
#define INC_2PC_TRANSACTIONCONTROLLER_H


#include <atomic>

class TransactionController {
public:

  static int get();

private:
  static std::atomic<int> id_;
};









#endif //INC_2PC_TRANSACTIONCONTROLLER_H
