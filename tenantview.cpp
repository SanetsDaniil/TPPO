#include "tenantview.h"
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QHeaderView>

TenantView::TenantView(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    table = new QTableWidget(this);

    // Настройка политики размера для растягивания
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(table);

    setLayout(layout);

    loadTenants();
}

void TenantView::loadTenants() {
    TenantManager manager;
    QVector<Tenant> tenants = manager.getAllTenants();

    table->setRowCount(tenants.size());
    table->setColumnCount(5); // Количество столбцов
    table->setHorizontalHeaderLabels({"ID", "Имя арендатора", "ID недвижимости", "Дата начала", "Дата окончания"});

    // Ожидаем, что таблица будет растягиваться, так что нужно немного настроить заголовки
    table->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < tenants.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(tenants[i].id)));
        table->setItem(i, 1, new QTableWidgetItem(tenants[i].name));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(tenants[i].propertyId)));
        table->setItem(i, 3, new QTableWidgetItem(tenants[i].leaseStart.toString("yyyy-MM-dd")));
        table->setItem(i, 4, new QTableWidgetItem(tenants[i].leaseEnd.toString("yyyy-MM-dd")));
    }
}
