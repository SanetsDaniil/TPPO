#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include "transaction.h"
#include <QVector>

/// Собирает данные для отчётов
class ReportManager {
public:
    ReportManager();
    /// Возвращает все сделки типа "Аренда"
    QVector<Transaction> getRentalTransactions();
    QVector<Transaction> getSalesTransactions();

};

#endif // REPORTMANAGER_H
