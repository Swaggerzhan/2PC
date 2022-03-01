//
// Created by swagger on 2022/2/27.
//

#include "TransactionController.h"
#include <random>

std::atomic<int> TransactionID::id_ = {};

int TransactionID::get() {
  return id_.fetch_add(1);
}


// 3 - 5
int TimeoutGenerator() {
  return (rand() % 2) + 3;
}

