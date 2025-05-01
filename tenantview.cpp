#include "tenantview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDate>

TenantView::TenantView(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);

    // Фильтры
    auto *filterLayout = new QHBoxLayout;
    typeFilterCombo = new QComboBox(this);
    typeFilterCombo->addItems({"Все", "Арендатор", "Покупатель"});
    filterLayout->addWidget(typeFilterCombo);

    nameFilterEdit = new QLineEdit(this);
    nameFilterEdit->setPlaceholderText("Фильтр по имени...");
    filterLayout->addWidget(nameFilterEdit);

    mainLayout->addLayout(filterLayout);

    // Таблица
    table = new QTableWidget(this);
    mainLayout->addWidget(table);

    // Кнопка удаления
    deleteButton = new QPushButton("Удалить арендатора", this);
    mainLayout->addWidget(deleteButton);

    setLayout(mainLayout);

    connect(typeFilterCombo, &QComboBox::currentTextChanged,    this, &TenantView::applyFilters);
    connect(nameFilterEdit,    &QLineEdit::textChanged,         this, &TenantView::applyFilters);
    connect(deleteButton,      &QPushButton::clicked,           this, &TenantView::deleteTenant);

    loadTenants();
}

void TenantView::loadTenants() {
    TenantManager mgr;
    auto tenants = mgr.getAllTenants();

    table->setRowCount(tenants.size());
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({
        "ID", "Тип", "Имя", "Email", "Дата рождения", "Телефон", "Дней до окончания"
    });
    table->horizontalHeader()->setStretchLastSection(true);

    QDate today = QDate::currentDate();
    auto db = DatabaseManager::instance().getDatabase();

    for (int i = 0; i < tenants.size(); ++i) {
        const auto &t = tenants[i];
        table->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        table->setItem(i, 1, new QTableWidgetItem(t.type));
        table->setItem(i, 2, new QTableWidgetItem(t.name));
        table->setItem(i, 3, new QTableWidgetItem(t.email));
        table->setItem(i, 4, new QTableWidgetItem(t.birthDate.toString("yyyy-MM-dd")));
        table->setItem(i, 5, new QTableWidgetItem(t.phone));

        // --- запросим ближайшее lease_end из transactions ---
        QSqlQuery q(db);
        q.prepare(R"(
            SELECT lease_end
              FROM transactions
             WHERE tenant_id = ?
               AND type = 'Аренда'
               AND DATE(lease_end) >= DATE('now')
             ORDER BY lease_end ASC
             LIMIT 1
        )");
        q.addBindValue(t.id);
        QDate leaseEnd;
        if (q.exec() && q.next()) {
            leaseEnd = q.value(0).toDate();
        }

        int daysLeft = leaseEnd.isValid()
                           ? today.daysTo(leaseEnd)
                           : std::numeric_limits<int>::max();

        QString daysText = leaseEnd.isValid()
                               ? QString::number(daysLeft)
                               : QString("—");

        auto *item = new QTableWidgetItem(daysText);
        table->setItem(i, 6, item);

        // подсветка
        if (leaseEnd.isValid() && daysLeft >= 0 && daysLeft <= WARNING_DAYS) {
            // предупреждение
            for (int c = 0; c < table->columnCount(); ++c)
                table->item(i,c)->setBackground(Qt::blue);
        } else if (leaseEnd.isValid() && daysLeft < 0) {
            // просрочено
            for (int c = 0; c < table->columnCount(); ++c)
                table->item(i,c)->setBackground(Qt::red);
        }
    }

    applyFilters();
}

void TenantView::applyFilters() {
    QString typeF = typeFilterCombo->currentText();
    QString nameF = nameFilterEdit->text().trimmed();

    for (int r = 0; r < table->rowCount(); ++r) {
        bool ok = true;
        if (typeF != "Все" && table->item(r,1)->text() != typeF)
            ok = false;
        if (ok && !nameF.isEmpty() &&
            !table->item(r,2)->text().contains(nameF, Qt::CaseInsensitive))
            ok = false;
        table->setRowHidden(r, !ok);
    }
}

void TenantView::deleteTenant() {
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите арендатора");
        return;
    }
    int id = table->item(row,0)->text().toInt();
    auto ret = QMessageBox::question(this, "Удалить?",
                                     "Удалить выбранного арендатора?",
                                     QMessageBox::Yes|QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    QSqlQuery q(DatabaseManager::instance().getDatabase());
    q.prepare("DELETE FROM tenants WHERE id = ?");
    q.addBindValue(id);
    if (q.exec()) {
        table->removeRow(row);
        QMessageBox::information(this, "Готово", "Арендатор удалён");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить арендатора");
    }
}
