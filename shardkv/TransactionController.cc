//
// Created by swagger on 2022/2/27.
//

#include "TransactionController.h"


std::atomic<int> TransactionController::id_ = {};

int TransactionController::get() {
  return id_.fetch_add(1);
}

