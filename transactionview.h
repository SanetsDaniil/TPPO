#ifndef TRANSACTIONVIEW_H
#define TRANSACTIONVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include "transactionmanager.h"

class TransactionView : public QWidget {
    Q_OBJECT
public:
    explicit TransactionView(QWidget *parent = nullptr);
    void loadTransactions();

private slots:
    void applyFilters();        // слот для применения фильтров
    void deleteCurrentTransaction();

private:
    QTableWidget *table;

    // Фильтры
    QComboBox *typeFilterCombo;    // Все / Продажа / Аренда
    QLineEdit *propertyFilterEdit; // По названию недвижимости
    QLineEdit *tenantFilterEdit;   // По имени арендатора
    QPushButton   *deleteButton;

    // Храним исходный список, чтобы фильтровать его
    QVector<Transaction> transactions;
};

#endif // TRANSACTIONVIEW_H
