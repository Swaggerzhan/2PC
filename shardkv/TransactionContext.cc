//
// Created by swagger on 2022/2/27.
//


#include "TransactionContext.h"
#include "TransactionController.h"


TransactionContext::TransactionContext()
: t_id(TransactionController::get())
{}


