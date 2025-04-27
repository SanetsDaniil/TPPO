#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H

#include "transaction.h"
#include <QSqlDatabase>
#include <QVector>

class TransactionManager {
public:
    TransactionManager();
    bool addTransaction(const Transaction &t);
    QVector<Transaction> getAllTransactions();
private:
    QSqlDatabase db;
};

#endif // TRANSACTIONMANAGER_H
