#ifndef TRANSACTIONVIEW_H
#define TRANSACTIONVIEW_H

#include <QWidget>
#include <QTableWidget>
#include "transactionmanager.h"

class TransactionView : public QWidget {
    Q_OBJECT
public:
    explicit TransactionView(QWidget *parent = nullptr);
    void loadTransactions();
private:
    QTableWidget *table;
};

#endif // TRANSACTIONVIEW_H
