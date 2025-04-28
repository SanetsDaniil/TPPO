#include "transactionview.h"
#include "propertymanager.h"
#include "tenantmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDebug>
#include <QMessageBox>

TransactionView::TransactionView(QWidget *parent): QWidget(parent) {
    // Общий лэйаут
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 1) Фильтры
    QHBoxLayout *filterLayout = new QHBoxLayout;
    typeFilterCombo    = new QComboBox(this);
    propertyFilterEdit = new QLineEdit(this);
    tenantFilterEdit   = new QLineEdit(this);

    typeFilterCombo->addItems({"Все", "Продажа", "Аренда"});
    propertyFilterEdit->setPlaceholderText("Фильтр по недвижимости...");
    tenantFilterEdit->setPlaceholderText("Фильтр по арендатору...");

    filterLayout->addWidget(typeFilterCombo);
    filterLayout->addWidget(propertyFilterEdit);
    filterLayout->addWidget(tenantFilterEdit);
    mainLayout->addLayout(filterLayout);

    // 2) Таблица
    table = new QTableWidget(this);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(table);

    // 3) (опционально) кнопка удаления
    deleteButton = new QPushButton("Удалить сделку", this);
    mainLayout->addWidget(deleteButton);

    setLayout(mainLayout);

    // Сигналы фильтров
    connect(typeFilterCombo,    &QComboBox::currentTextChanged, this, &TransactionView::applyFilters);
    connect(propertyFilterEdit, &QLineEdit::textChanged, this, &TransactionView::applyFilters);
    connect(tenantFilterEdit,   &QLineEdit::textChanged, this, &TransactionView::applyFilters);
    connect(deleteButton, &QPushButton::clicked, this, &TransactionView::deleteCurrentTransaction);

    // Загружаем и отображаем
    loadTransactions();
}

void TransactionView::loadTransactions() {
    // 1) Получаем исходный список из БД
    TransactionManager mgr;
    transactions = mgr.getAllTransactions();

    // 2) Подготавливаем таблицу
    table->setRowCount(transactions.size());
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(
        {"ID", "Недвижимость", "Арендатор", "Дата", "Тип", "Сумма"}
        );

    // Загрузим справочники для отображения имён
    PropertyManager pm;
    auto props = pm.getAllProperties();
    TenantManager tm;
    auto tenants = tm.getAllTenants();

    // 3) Заполняем строки
    for (int i = 0; i < transactions.size(); ++i) {
        const auto &t = transactions[i];

        // Находим имя недвижимости
        // QString propName;
        // for (auto &p : props) if (p.id == t.propertyId) { propName = p.name; break; }
        QString propName = t.propertyName;

        // Находим имя арендатора (может быть пусто)
        // QString tenantName;
        // if (t.tenantId > 0) {
        //     for (auto &te : tenants) if (te.id == t.tenantId) {
        //             tenantName = te.name; break;
        //         }
        // }
        QString tenantName = t.tenantName;

        table->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        table->setItem(i, 1, new QTableWidgetItem(propName));
        table->setItem(i, 2, new QTableWidgetItem(tenantName));
        table->setItem(i, 3, new QTableWidgetItem(t.date.toString("yyyy-MM-dd")));
        table->setItem(i, 4, new QTableWidgetItem(t.type));
        table->setItem(i, 5, new QTableWidgetItem(QString::number(t.amount, 'f', 2)));
    }

    table->horizontalHeader()->setStretchLastSection(true);
    table->resizeColumnsToContents();

    // Сразу применяем фильтры (например, если заданы по умолчанию)
    applyFilters();
}

void TransactionView::applyFilters() {
    QString typeFilter = typeFilterCombo->currentText();
    QString propFilter = propertyFilterEdit->text().trimmed();
    QString tenFilter  = tenantFilterEdit->text().trimmed();

    for (int row = 0; row < table->rowCount(); ++row) {
        bool visible = true;

        // 1) Фильтр по типу
        if (typeFilter != "Все") {
            QString cellType = table->item(row, 4)->text();
            if (cellType != typeFilter) visible = false;
        }

        // 2) Фильтр по недвижимости
        if (visible && !propFilter.isEmpty()) {
            QString cellProp = table->item(row, 1)->text();
            if (!cellProp.contains(propFilter, Qt::CaseInsensitive)) visible = false;
        }

        // 3) Фильтр по арендатору
        if (visible && !tenFilter.isEmpty()) {
            QString cellTen = table->item(row, 2)->text();
            if (!cellTen.contains(tenFilter, Qt::CaseInsensitive)) visible = false;
        }

        table->setRowHidden(row, !visible);
    }
}

void TransactionView::deleteCurrentTransaction() {
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите сделку для удаления.");
        return;
    }
    int transId = table->item(row, 0)->text().toInt();
    // вызываем менеджер
    TransactionManager mgr;
    if (!mgr.removeTransaction(transId)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось удалить сделку из базы.");
        return;
    }
    // удаляем из таблицы
    table->removeRow(row);
    QMessageBox::information(this, "Готово", "Сделка удалена.");
}
