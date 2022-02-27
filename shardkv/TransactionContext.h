//
// Created by swagger on 2022/2/27.
//

#ifndef INC_2PC_TRANSACTIONCONTEXT_H
#define INC_2PC_TRANSACTIONCONTEXT_H


#include <vector>
#include <memory>

struct Entry;

enum UndoType {
  Delete,
  Replace,
  Insert,
};

// key -> value
typedef std::pair<std::string, std::shared_ptr<Entry>> Record;
typedef std::pair<UndoType, Record> UndoRecord;

struct TransactionContext {

  TransactionContext();

  int t_id;
  std::vector<UndoRecord> undoList;
};




#endif //INC_2PC_TRANSACTIONCONTEXT_H
