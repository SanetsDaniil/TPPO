#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "propertyview.h"
#include "propertymanager.h"
#include "tenantmanager.h"
#include "tenantview.h"
#include "database.h"
#include "transactionview.h"
#include "transactionmanager.h"
#include "reportmanager.h"


#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QtPrintSupport/QPrinter>
#include <QFileDialog>
#include <QHeaderView>
#include <QTextDocument>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (!DatabaseManager::instance().openDatabase()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
}

    PropertyManager().releaseExpiredLeases();

    setupToolbar();

    notifyExpiringLeases();
    showPropertyList();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupToolbar()
{
    QToolBar *toolbar = addToolBar("Навигация");

    QAction *viewPropertiesAction = new QAction("Недвижимость", this);
    QAction *addTenantAction = new QAction("Добавить арендатора", this);
    QAction *addPropertyAction = new QAction("Добавить недвижимость", this);
    QAction *viewTenantAction = new QAction("Арендаторы", this);
    QAction *viewTransactionAction = new QAction("Сделки", this);
    QAction *addTransactionAction = new QAction("Новая сделка", this);
    QAction *rentalReportAction = new QAction("Отчёт по аренде", this);
    QAction *salesReportAction = new QAction("Отчёт по продажам", this);

    toolbar->addAction(viewPropertiesAction);
    toolbar->addAction(viewTenantAction);
    toolbar->addAction(addTenantAction);
    toolbar->addAction(addPropertyAction);
    toolbar->addAction(viewTransactionAction);
    toolbar->addAction(addTransactionAction);
    toolbar->addAction(rentalReportAction);
    toolbar->addAction(salesReportAction);

    connect(viewPropertiesAction, &QAction::triggered, this, &MainWindow::showPropertyList);
    connect(viewTenantAction,&QAction::triggered, this, &MainWindow::showTenantList);
    connect(addTenantAction, &QAction::triggered, this, &MainWindow::showTenantForm);
    connect(addPropertyAction, &QAction::triggered, this, &MainWindow::showPropertyForm);
    connect(viewTransactionAction, &QAction::triggered, this, &MainWindow::showTransactionList);
    connect(addTransactionAction, &QAction::triggered, this, &MainWindow::showTransactionForm);
    connect(rentalReportAction, &QAction::triggered, this, &MainWindow::showRentalReport);
    connect(salesReportAction, &QAction::triggered, this, &MainWindow::showSalesReport);
}


void MainWindow::showPropertyList()
{
    PropertyView *view = new PropertyView(this);
    setCentralWidget(view);
}

void MainWindow::showPropertyForm() {
    // Модальное окно
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Добавить/Редактировать недвижимость");
    QVBoxLayout *vlay = new QVBoxLayout(dlg);
    QFormLayout  *form = new QFormLayout;

    // Поля формы
    QLineEdit   *nameEdit     = new QLineEdit(dlg);
    QComboBox   *typeCombo    = new QComboBox(dlg);
    typeCombo->addItems({"Квартира","Дом","Офис","Склад"});
    QLineEdit   *locationEdit = new QLineEdit(dlg);
    QComboBox   *statusCombo  = new QComboBox(dlg);
    statusCombo->addItems({"Свободно","Занято","Продано"});
    QLineEdit   *priceEdit    = new QLineEdit(dlg);

    form->addRow("Название", nameEdit);
    form->addRow("Тип",      typeCombo);
    form->addRow("Локация",  locationEdit);
    form->addRow("Статус",   statusCombo);
    form->addRow("Цена",     priceEdit);
    vlay->addLayout(form);

    QPushButton *saveBtn = new QPushButton("Сохранить", dlg);
    vlay->addWidget(saveBtn);

    dlg->setLayout(vlay);
    dlg->resize(320, 260);

    // При клике «Сохранить» собираем данные и сохраняем
    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        Property p;
        p.name     = nameEdit->text();
        p.type     = typeCombo->currentText();
        p.location = locationEdit->text();
        p.status   = statusCombo->currentText();
        p.price    = priceEdit->text().toDouble();

        PropertyManager mgr;
        if (!mgr.addProperty(p)) {
            QMessageBox::warning(dlg, "Ошибка", "Не удалось сохранить недвижимость");
        } else {
            QMessageBox::information(dlg, "Готово", "Недвижимость сохранена");
            dlg->accept();
        }
    });

    dlg->exec();
}

void MainWindow::on_addPropertyButton_clicked() {
    // Просто вызываем форму
    showPropertyForm();
}


void MainWindow::showTenantForm() {
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Добавить/Редактировать арендатора");

    QFormLayout *form = new QFormLayout(dlg);
    QVBoxLayout *vlay = new QVBoxLayout(dlg);

    // 1) Тип
    QComboBox *typeCombo = new QComboBox(dlg);
    typeCombo->addItems({"Арендатор","Покупатель"});
    form->addRow("Тип:", typeCombo);

    // 2) Личные данные
    QLineEdit *nameEdit      = new QLineEdit(dlg);
    QLineEdit *emailEdit     = new QLineEdit(dlg);
    QDateEdit *birthDateEdit = new QDateEdit(QDate::currentDate(), dlg);
    birthDateEdit->setCalendarPopup(true);
    QLineEdit *phoneEdit     = new QLineEdit(dlg);

    form->addRow("Имя:",           nameEdit);
    form->addRow("Email:",         emailEdit);
    form->addRow("Дата рождения:", birthDateEdit);
    form->addRow("Телефон:",       phoneEdit);

    // 3) Кнопка «Сохранить»
    QPushButton *saveBtn = new QPushButton("Сохранить", dlg);
    form->addRow(saveBtn);

    vlay->addLayout(form);
    dlg->setLayout(vlay);
    dlg->resize(360, 240);

    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        Tenant t;
        t.type      = typeCombo->currentText();
        t.name      = nameEdit->text().trimmed();
        t.email     = emailEdit->text().trimmed();
        t.birthDate = birthDateEdit->date();
        t.phone     = phoneEdit->text().trimmed();

        // Валидация
        if (t.name.isEmpty()) {
            QMessageBox::warning(dlg, "Ошибка", "Имя не может быть пустым");
            return;
        }
        if (t.phone.isEmpty()) {
            QMessageBox::warning(dlg, "Ошибка", "Телефон не может быть пустым");
            return;
        }

        TenantManager tmgr;
        int newId = tmgr.addTenant(t);  // см. вариант A: addTenant возвращает новый ID
        if (newId < 0) {
            QMessageBox::warning(dlg, "Ошибка", "Не удалось сохранить арендатора");
            return;
        }

        QMessageBox::information(dlg, "Готово", "Арендатор сохранён");
        dlg->accept();

        if (auto tv = qobject_cast<TenantView*>(centralWidget()))
            tv->loadTenants();
    });

    dlg->exec();
}


void MainWindow::on_addTenantButton_clicked() {
    showTenantForm();
}

void MainWindow::showTenantList()
{
    TenantView *view = new TenantView(this);
    setCentralWidget(view);
}



void MainWindow::loadPropertiesToComboBox() {
    PropertyManager manager;
    currentProperties = manager.getAllProperties();

    editPropertyComboBox->clear();
    editPropertyComboBox->addItem("Новая недвижимость");

    for (const Property &p : currentProperties) {
        editPropertyComboBox->addItem(p.name);
    }

    editPropertyComboBox->setCurrentIndex(0);
    //clearPropertyFormFields(); // твоя существующая функция, если она есть
}

void MainWindow::fillPropertyFieldsFromSelected() {
    int index = editPropertyComboBox->currentIndex();
    if (index == 0) {
        // Новая недвижимость
        propertyNameLineEdit->clear();
        propertyTypeComboBox->setCurrentIndex(0);
        propertyLocationLineEdit->clear();
        propertyStatusComboBox->setCurrentIndex(0);
        propertyPriceLineEdit->clear();
    } else if (index > 0 && index - 1 < currentProperties.size()) {
        const Property &p = currentProperties[index - 1];
        propertyNameLineEdit->setText(p.name);
        propertyTypeComboBox->setCurrentText(p.type);
        propertyLocationLineEdit->setText(p.location);
        propertyStatusComboBox->setCurrentText(p.status);
        propertyPriceLineEdit->setText(QString::number(p.price));
    }
}

void MainWindow::showTransactionList() {
    auto *view = new TransactionView(this);
    setCentralWidget(view);
}

void MainWindow::showTransactionForm() {
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Добавить/Редактировать сделку");

    QFormLayout *form = new QFormLayout;
    QVBoxLayout *vlay = new QVBoxLayout(dlg);

    // 1) Недвижимость
    QComboBox *propCombo = new QComboBox(dlg);
    for (const auto &p : PropertyManager().getFreeProperties())
        propCombo->addItem(QString("%1 (ID:%2)").arg(p.name).arg(p.id), p.id);

    form->addRow("Недвижимость:", propCombo);

    // 2) Арендатор
    QComboBox *tenantCombo = new QComboBox(dlg);
    tenantCombo->addItem("— нет —", 0);
    for (const auto &t : TenantManager().getAllTenants())
        tenantCombo->addItem(QString("%1 (ID:%2)").arg(t.name).arg(t.id), t.id);
    form->addRow("Арендатор:", tenantCombo);

    // 3) Тип сделки
    QComboBox *typeCombo = new QComboBox(dlg);
    typeCombo->addItems({"Продажа","Аренда"});
    form->addRow("Тип сделки:", typeCombo);

    // 4) Дата сделки
    QDateEdit *dateEdit = new QDateEdit(QDate::currentDate(), dlg);
    dateEdit->setCalendarPopup(true);
    form->addRow("Дата:", dateEdit);

    // 5) Даты аренды + сумма
    QDateEdit *leaseStart = new QDateEdit(QDate::currentDate(), dlg);
    leaseStart->setCalendarPopup(true);
    QDateEdit *leaseEnd = new QDateEdit(QDate::currentDate().addMonths(1), dlg);
    leaseEnd->setCalendarPopup(true);
    QLabel *sumLabel = new QLabel("Сумма: 0.00", dlg);

    form->addRow("Начало аренды:", leaseStart);
    form->addRow("Окончание аренды:", leaseEnd);
    form->addRow("К оплате:", sumLabel);

    // Скрываем поля дат и суммы при «Продажа»
    auto updateRentalFields = [=]() {
        bool rent = (typeCombo->currentText() == "Аренда");
        leaseStart->setVisible(rent);
        leaseEnd->setVisible(rent);
        sumLabel->setVisible(rent);
    };
    connect(typeCombo, &QComboBox::currentTextChanged, dlg, updateRentalFields);
    updateRentalFields();

    // Лямбда для пересчёта суммы аренды
    auto recalcSum = [=]() {
        // Получаем выбранное ID недвижимости
        int pid = propCombo->currentData().toInt();
        // Ищем его в менеджере, чтобы достать price
        double rate = 0;
        for (const auto &p : PropertyManager().getAllProperties()) {
            if (p.id == pid) {
                rate = p.price;
                break;
            }
        }
        // Считаем месяцы
        int months = leaseStart->date().daysTo(leaseEnd->date()) / 30;
        if (months < 1) months = 1;
        double total = rate * months;
        sumLabel->setText(QString("Сумма: %1").arg(total, 0, 'f', 2));
    };

    // Пересчитываем, когда меняется недвижимость или даты или тип
    connect(propCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), dlg, recalcSum);
    connect(leaseStart, &QDateEdit::dateChanged, dlg, recalcSum);
    connect(leaseEnd, &QDateEdit::dateChanged, dlg, recalcSum);
    connect(typeCombo, &QComboBox::currentTextChanged, dlg, recalcSum);
    recalcSum();

    // 6) Кнопка «Сохранить»
    QPushButton *saveBtn = new QPushButton("Сохранить", dlg);
    form->addRow(saveBtn);

    vlay->addLayout(form);
    dlg->setLayout(vlay);
    dlg->resize(400, 360);

    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        Transaction t;
        t.propertyId   = propCombo->currentData().toInt();
        t.tenantId     = tenantCombo->currentData().toInt();
        t.type         = typeCombo->currentText();
        t.date         = dateEdit->date();
        t.propertyName = propCombo->currentText().split(" (ID").first();
        t.tenantName   = (t.tenantId > 0
                            ? tenantCombo->currentText().split(" (ID").first()
                            : QString("—"));

        if (t.type == "Аренда") {
            t.leaseStart = leaseStart->date();
            t.leaseEnd   = leaseEnd->date();
            // Парсим сумму из лейбла
            t.amount = sumLabel->text().split(' ').last().toDouble();
        } else {
            bool ok;
            double sale = QInputDialog::getDouble(
                dlg, "Сумма продажи", "Введите сумму:", 0, 0, 1e12, 2, &ok
                );
            if (!ok || sale <= 0) {
                QMessageBox::warning(dlg, "Ошибка", "Неверная сумма продажи!");
                return;
            }
            t.amount = sale;
        }

        // Валидация
        if (t.propertyId <= 0) {
            QMessageBox::warning(dlg, "Ошибка", "Выберите недвижимость!");
            return;
        }

        TransactionManager mgr;
        if (mgr.addTransaction(t)) {
            QMessageBox::information(dlg, "Готово", "Сделка сохранена");
            dlg->accept();
            showTransactionList();
        } else {
            QMessageBox::critical(dlg, "Ошибка", "Не удалось сохранить сделку");
        }
        PropertyManager pm;
        if (t.type == "Аренда") {
            pm.updateStatus(t.propertyId, "Занято");
        } else { // Продажа
            pm.updateStatus(t.propertyId, "Продано");
        }
    });

    dlg->exec();
}



void MainWindow::on_addTransactionButton_clicked() {
    Transaction t;
    t.propertyId = transactionPropertyCombo->currentData().toInt();
    t.tenantId   = transactionTenantCombo->currentData().toInt();
    t.type       = transactionTypeCombo->currentText();
    t.date       = transactionDateEdit->date();

    // Заполняем поля для отображения
    t.propertyName = transactionPropertyCombo->currentText();
    t.tenantName   = transactionTenantCombo->currentText();

    // Если аренда — рассчитываем автоматически по цене недвижимости и сроку
    if (t.type == "Аренда") {
        t.leaseStart = transactionLeaseStartEdit->date();
        t.leaseEnd   = transactionLeaseEndEdit->date();

        // Найдём цену из Property.price
        double rate = 0;
        for (auto &p : PropertyManager().getAllProperties())
            if (p.id == t.propertyId) { rate = p.price; break; }

        int months = t.leaseStart.daysTo(t.leaseEnd) / 30;
        if (months < 1) months = 1;
        t.amount = rate * months;
    } else {
        t.amount = transactionAmountEdit->text().toDouble();
    }

    // Валидация
    if (t.propertyId <= 0 || t.amount <= 0.0) {
        QMessageBox::warning(nullptr, "Ошибка", "Проверьте недвижимость и сумму сделки");
        return;
    }

    TransactionManager mgr;
    if (mgr.addTransaction(t)) {
        QMessageBox::information(nullptr, "Готово", "Сделка сохранена");
        showTransactionList();
    } else {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось сохранить сделку");
    }
}

void MainWindow::showRentalReport() {
    // Диалог отчёта
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Отчёт по аренде");
    QVBoxLayout *vlay = new QVBoxLayout(dlg);

    // Таблица
    QTableWidget *table = new QTableWidget(dlg);
    vlay->addWidget(table);

    // Кнопки: «Экспорт в PDF» и «Закрыть»
    QHBoxLayout *hl = new QHBoxLayout;
    QPushButton *exportBtn = new QPushButton("Экспорт в PDF", dlg);
    QPushButton *closeBtn  = new QPushButton("Закрыть", dlg);
    hl->addWidget(exportBtn);
    hl->addStretch();
    hl->addWidget(closeBtn);
    vlay->addLayout(hl);

    // Заполняем табличку данными
    ReportManager rm;
    auto rentals = rm.getRentalTransactions();
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(
        {"ID", "Недвижимость", "Арендатор", "Начало", "Окончание", "Сумма"}
        );
    table->setRowCount(rentals.size());
    for (int i = 0; i < rentals.size(); ++i) {
        const auto &t = rentals[i];
        table->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        table->setItem(i, 1, new QTableWidgetItem(t.propertyName));
        table->setItem(i, 2, new QTableWidgetItem(t.tenantName));
        table->setItem(i, 3, new QTableWidgetItem(t.leaseStart.toString("yyyy-MM-dd")));
        table->setItem(i, 4, new QTableWidgetItem(t.leaseEnd.toString("yyyy-MM-dd")));
        table->setItem(i, 5, new QTableWidgetItem(QString::number(t.amount, 'f', 2)));
    }
    table->resizeColumnsToContents();

    // Экспорт в PDF
    connect(exportBtn, &QPushButton::clicked, dlg, [=]() {
        QString path = QFileDialog::getSaveFileName(
            dlg, "Сохранить отчёт", "", "PDF-файл (*.pdf)"
            );
        if (path.isEmpty()) return;

        // Собираем HTML таблицу
        QString html = "<h2>Отчёт по аренде</h2>";
        html += "<table border='1' cellspacing='0' cellpadding='4'>";
        html += "<tr>";
        for (int c = 0; c < table->columnCount(); ++c)
            html += "<th>" + table->horizontalHeaderItem(c)->text() + "</th>";
        html += "</tr>";
        for (int r = 0; r < table->rowCount(); ++r) {
            html += "<tr>";
            for (int c = 0; c < table->columnCount(); ++c) {
                html += "<td>" + table->item(r, c)->text() + "</td>";
            }
            html += "</tr>";
        }
        html += "</table>";

        // Печать в PDF
        QTextDocument doc;
        doc.setHtml(html);
        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(path);
        doc.print(&printer);

        QMessageBox::information(dlg, "Готово", "Отчёт сохранён в PDF");
    });

    connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);

    dlg->resize(800, 600);
    dlg->exec();
}

void MainWindow::showSalesReport() {
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Отчёт по продажам");
    QVBoxLayout *vlay = new QVBoxLayout(dlg);

    auto *table = new QTableWidget(dlg);
    vlay->addWidget(table);

    QHBoxLayout *hl = new QHBoxLayout;
    auto *exportBtn = new QPushButton("Экспорт в PDF", dlg);
    auto *closeBtn  = new QPushButton("Закрыть", dlg);
    hl->addWidget(exportBtn);
    hl->addStretch();
    hl->addWidget(closeBtn);
    vlay->addLayout(hl);

    // Заполняем данными
    ReportManager rm;
    auto sales = rm.getSalesTransactions();
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels(
        {"ID","Недвижимость","Покупатель","Дата","Тип","Сумма"}
        );
    table->setRowCount(sales.size());
    for (int i = 0; i < sales.size(); ++i) {
        const auto &t = sales[i];
        table->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        table->setItem(i, 1, new QTableWidgetItem(t.propertyName));
        table->setItem(i, 2, new QTableWidgetItem(t.tenantName));
        table->setItem(i, 3, new QTableWidgetItem(t.date.toString("yyyy-MM-dd")));
        table->setItem(i, 4, new QTableWidgetItem(t.type));
        table->setItem(i, 5, new QTableWidgetItem(QString::number(t.amount,'f',2)));
    }
    table->horizontalHeader()->setStretchLastSection(true);
    table->resizeColumnsToContents();

    // Экспорт PDF
    connect(exportBtn, &QPushButton::clicked, dlg, [=]() {
        QString path = QFileDialog::getSaveFileName(
            dlg, "Сохранить отчёт", "", "PDF-файл (*.pdf)"
            );
        if (path.isEmpty()) return;

        QString html = "<h2>Отчёт по продажам</h2><table border='1' cellspacing='0' cellpadding='4'><tr>";
        for (int c = 0; c < table->columnCount(); ++c)
            html += "<th>" + table->horizontalHeaderItem(c)->text() + "</th>";
        html += "</tr>";
        for (int r = 0; r < table->rowCount(); ++r) {
            html += "<tr>";
            for (int c = 0; c < table->columnCount(); ++c)
                html += "<td>" + table->item(r,c)->text() + "</td>";
            html += "</tr>";
        }
        html += "</table>";

        QTextDocument doc;
        doc.setHtml(html);
        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(path);
        doc.print(&printer);

        QMessageBox::information(dlg, "Готово", "Отчёт сохранён в PDF");
    });

    connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);

    dlg->resize(800,600);
    dlg->exec();
}

void MainWindow::notifyExpiringLeases() {
    TenantManager tm;
    auto tenants = tm.getAllTenants();

    const int threshold = 30; // дней до окончания для уведомления
    QDate today = QDate::currentDate();
    QStringList soon;

    auto db = DatabaseManager::instance().getDatabase();
    for (const auto &t : tenants) {
        if (t.type != "Арендатор")
            continue;

        // Ищем ближайшую активную аренду по этому арендатору
        QSqlQuery q(db);
        q.prepare(R"(
            SELECT lease_end
              FROM transactions
             WHERE tenant_id = :id
               AND type = 'Аренда'
               AND DATE(lease_end) >= DATE('now')
             ORDER BY lease_end ASC
             LIMIT 1
        )");
        q.bindValue(":id", t.id);

        if (!q.exec()) {
            qDebug() << "Ошибка при выборке lease_end:" << q.lastError().text();
            continue;
        }
        if (!q.next()) {
            // Нет активных аренды — пропускаем
            continue;
        }

        QDate leaseEnd = q.value(0).toDate();
        int daysLeft = today.daysTo(leaseEnd);
        if (daysLeft >= 0 && daysLeft <= threshold) {
            soon << QString("%1 (ID:%2) — через %3 дн.")
                        .arg(t.name).arg(t.id).arg(daysLeft);
        }
    }

    if (!soon.isEmpty()) {
        QMessageBox::information(
            this,
            "Скоро окончатся договоры",
            "Договоры аренды заканчиваются у следующих клиентов:\n\n"
                + soon.join("\n")
            );
    }
}
