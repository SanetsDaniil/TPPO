#include "transactionview.h"
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDebug>

TransactionView::TransactionView(QWidget *parent): QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    table = new QTableWidget(this);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(table);
    setLayout(layout);
    loadTransactions();
}

void TransactionView::loadTransactions() {
    TransactionManager mgr;
    auto list = mgr.getAllTransactions();
    table->setRowCount(list.size());
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(
        QStringList {"ID", "ID недвижимости", "ID арендатора", "Дата", "Тип", "Сумма"}
        );
    for (int i = 0; i < list.size(); ++i) {
        const auto &t = list[i];
        table->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(t.propertyId)));
        // tenantId может быть 0 => пустая строка
        table->setItem(i, 2, new QTableWidgetItem(t.tenantId>0
                                                      ? QString::number(t.tenantId) : QString("")));
        table->setItem(i, 3, new QTableWidgetItem(t.date.toString("yyyy-MM-dd")));
        table->setItem(i, 4, new QTableWidgetItem(t.type));
        table->setItem(i, 5, new QTableWidgetItem(QString::number(t.amount, 'f', 2)));
    }
    table->horizontalHeader()->setStretchLastSection(true);
    table->resizeColumnsToContents();
}
