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

    // Фильтр по телефону
    phoneFilterEdit = new QLineEdit(this);
    phoneFilterEdit->setPlaceholderText("Фильтр по телефону...");
    filterLayout->addWidget(phoneFilterEdit);

    mainLayout->addLayout(filterLayout);

    table = new QTableWidget(this);
    mainLayout->addWidget(table);

    deleteButton = new QPushButton("Удалить выбранного арендатора", this);
    mainLayout->addWidget(deleteButton);

    setLayout(mainLayout);

    loadTenants();

    connect(typeFilterCombo,   &QComboBox::currentTextChanged, this, &TenantView::applyFilters);
    connect(nameFilterEdit,    &QLineEdit::textChanged,        this, &TenantView::applyFilters);
    connect(phoneFilterEdit,   &QLineEdit::textChanged,        this, &TenantView::applyFilters);
    connect(deleteButton,      &QPushButton::clicked,          this, &TenantView::deleteTenant);
}

void TenantView::loadTenants() {
    TenantManager manager;
    QVector<Tenant> tenants = manager.getAllTenants();

    table->setRowCount(tenants.size());
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"ID", "Тип", "Имя", "Email", "Дата рождения", "Телефон"});

    table->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < tenants.size(); ++i) {
        const auto &t = tenants[i];
        table->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        table->setItem(i, 1, new QTableWidgetItem(t.type));
        table->setItem(i, 2, new QTableWidgetItem(t.name));
        table->setItem(i, 3, new QTableWidgetItem(t.email));
        table->setItem(i, 4, new QTableWidgetItem(t.birthDate.toString("yyyy-MM-dd")));
        table->setItem(i, 5, new QTableWidgetItem(t.phone));
    }

    applyFilters();
}


void TenantView::applyFilters() {
    QString typeFilter = typeFilterCombo->currentText();
    QString nameFilter = nameFilterEdit->text().trimmed();
    QString phoneFilter = phoneFilterEdit->text().trimmed();

    for (int i = 0; i < table->rowCount(); ++i) {
        bool visible = true;

        QString tenantType = table->item(i, 1)->text();
        QString tenantName = table->item(i, 2)->text();
        QString cellPhone = table->item(i, 5)->text();

        if (typeFilter != "Все" && tenantType != typeFilter)
            visible = false;

        if (visible && !nameFilter.isEmpty() && !tenantName.contains(nameFilter, Qt::CaseInsensitive))
            visible = false;

        if (!phoneFilter.isEmpty() && !cellPhone.contains(phoneFilter, Qt::CaseInsensitive))
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

    // Диалог подтверждения
    auto ret = QMessageBox::question(
        this, "Подтверждение",
        "Вы уверены, что хотите удалить этого арендатора?\nСделки с ним сохранятся с его именем.",
        QMessageBox::Yes|QMessageBox::No, QMessageBox::No
        );
    if (ret != QMessageBox::Yes) return;

    QSqlQuery query;
    query.prepare("DELETE FROM tenants WHERE id = ?");
    query.addBindValue(tenantId);
    if (query.exec()) {
        table->removeRow(row);
        QMessageBox::information(this, "Удалено", "Арендатор удалён.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить арендатора.");
    }
}

