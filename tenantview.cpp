#include "tenantview.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QHeaderView>

TenantView::TenantView(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Фильтры
    QHBoxLayout *filterLayout = new QHBoxLayout;
    typeFilterCombo = new QComboBox(this);
    typeFilterCombo->addItems({"Все", "Арендатор", "Покупатель"});
    filterLayout->addWidget(typeFilterCombo);

    nameFilterEdit = new QLineEdit(this);
    nameFilterEdit->setPlaceholderText("Фильтр по имени арендатора...");
    filterLayout->addWidget(nameFilterEdit);

    mainLayout->addLayout(filterLayout);

    table = new QTableWidget(this);
    mainLayout->addWidget(table);

    deleteButton = new QPushButton("Удалить выбранного арендатора", this);
    mainLayout->addWidget(deleteButton);

    setLayout(mainLayout);

    loadTenants();

    connect(typeFilterCombo,   &QComboBox::currentTextChanged, this, &TenantView::applyFilters);
    connect(nameFilterEdit,    &QLineEdit::textChanged,        this, &TenantView::applyFilters);
    connect(deleteButton,      &QPushButton::clicked,          this, &TenantView::deleteTenant);
}

void TenantView::loadTenants() {
    TenantManager manager;
    QVector<Tenant> tenants = manager.getAllTenants();

    table->setRowCount(tenants.size());
    table->setColumnCount(7); // теперь 7 столбцов
    table->setHorizontalHeaderLabels({"ID", "Тип", "Имя арендатора", "ID недвижимости", "Стоимость аренды", "Дата начала", "Дата окончания"});
    table->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < tenants.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(tenants[i].id)));
        table->setItem(i, 1, new QTableWidgetItem(tenants[i].type));
        table->setItem(i, 2, new QTableWidgetItem(tenants[i].name));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(tenants[i].propertyId)));
        table->setItem(i, 4, new QTableWidgetItem(QString::number(tenants[i].monthCost, 'f', 2)));
        table->setItem(i, 5, new QTableWidgetItem(tenants[i].leaseStart.toString("yyyy-MM-dd")));
        table->setItem(i, 6, new QTableWidgetItem(tenants[i].leaseEnd.toString("yyyy-MM-dd")));
    }

    applyFilters(); // После загрузки сразу применяем фильтры
}

void TenantView::applyFilters() {
    QString typeFilter = typeFilterCombo->currentText();
    QString nameFilter = nameFilterEdit->text().trimmed();

    for (int i = 0; i < table->rowCount(); ++i) {
        bool visible = true;

        QString tenantType = table->item(i, 1)->text();
        QString tenantName = table->item(i, 2)->text();

        if (typeFilter != "Все" && tenantType != typeFilter)
            visible = false;

        if (visible && !nameFilter.isEmpty() && !tenantName.contains(nameFilter, Qt::CaseInsensitive))
            visible = false;

        table->setRowHidden(i, !visible);
    }
}

void TenantView::deleteTenant() {
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите арендатора для удаления.");
        return;
    }

    int tenantId = table->item(row, 0)->text().toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM tenants WHERE id = ?");
    query.addBindValue(tenantId);

    if (query.exec()) {
        table->removeRow(row);
        QMessageBox::information(this, "Удаление", "Арендатор успешно удалён.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить арендатора.");
    }
}
