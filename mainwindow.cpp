#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "propertyview.h"
#include "propertymanager.h"
#include "tenantmanager.h"
#include "tenantview.h"
#include "database.h"
#include "transactionview.h"
#include "transactionmanager.h"


#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (!DatabaseManager::instance().openDatabase()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных!");
}

    setupToolbar();
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

    toolbar->addAction(viewPropertiesAction);
    toolbar->addAction(viewTenantAction);
    toolbar->addAction(addTenantAction);
    toolbar->addAction(addPropertyAction);
    toolbar->addAction(viewTransactionAction);
    toolbar->addAction(addTransactionAction);

    connect(viewPropertiesAction, &QAction::triggered, this, &MainWindow::showPropertyList);
    connect(viewTenantAction,&QAction::triggered, this, &MainWindow::showTenantList);
    connect(addTenantAction, &QAction::triggered, this, &MainWindow::showTenantForm);
    connect(addPropertyAction, &QAction::triggered, this, &MainWindow::showPropertyForm);
    connect(viewTransactionAction, &QAction::triggered, this, &MainWindow::showTransactionList);
    connect(addTransactionAction, &QAction::triggered, this, &MainWindow::showTransactionForm);
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
    QVBoxLayout *vlay = new QVBoxLayout(dlg);
    QFormLayout  *form = new QFormLayout;

    // Поля формы
    QComboBox *typeCombo      = new QComboBox(dlg);
    typeCombo->addItems({"Арендатор","Покупатель"});
    QLineEdit *nameEdit       = new QLineEdit(dlg);
    QComboBox *propertyCombo  = new QComboBox(dlg);
    for (auto &p : PropertyManager().getAllProperties())
        propertyCombo->addItem(QString("%1 (ID:%2)").arg(p.name).arg(p.id), p.id);
    QLineEdit *costEdit       = new QLineEdit(dlg);
    QDateEdit *startEdit      = new QDateEdit(QDate::currentDate(), dlg);
    startEdit->setCalendarPopup(true);
    QDateEdit *endEdit        = new QDateEdit(QDate::currentDate(), dlg);
    endEdit->setCalendarPopup(true);

    form->addRow("Тип",           typeCombo);
    form->addRow("Имя",           nameEdit);
    form->addRow("Недвижимость",  propertyCombo);
    form->addRow("Стоимость",     costEdit);
    form->addRow("Дата начала",   startEdit);
    form->addRow("Дата окончания",endEdit);
    vlay->addLayout(form);

    QPushButton *saveBtn = new QPushButton("Сохранить", dlg);
    vlay->addWidget(saveBtn);

    dlg->setLayout(vlay);
    dlg->resize(320, 300);

    // Скрываем поле даты окончания для покупателей
    connect(typeCombo, &QComboBox::currentTextChanged, dlg, [=](const QString &txt){
        endEdit->setVisible(txt == "Арендатор");
    });
    endEdit->setVisible(typeCombo->currentText() == "Арендатор");

    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        Tenant t;
        t.type       = typeCombo->currentText();
        t.name       = nameEdit->text();
        t.propertyId = propertyCombo->currentData().toInt();
        t.monthCost  = costEdit->text().toDouble();
        t.leaseStart = startEdit->date();
        t.leaseEnd   = (t.type == "Покупатель" ? QDate() : endEdit->date());

        TenantManager mgr;
        if (!mgr.addTenant(t)) {
            QMessageBox::warning(dlg, "Ошибка", "Не удалось сохранить арендатора");
        } else {
            QMessageBox::information(dlg, "Готово", "Арендатор сохранён");
            dlg->accept();
        }
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
    QWidget *form = new QWidget(nullptr, Qt::Window);  // отдельное окно
    form->setWindowTitle("Добавить/Изменить сделку");
    QVBoxLayout *layout = new QVBoxLayout(form);

    // 1) Выбор недвижимости
    layout->addWidget(new QLabel("Недвижимость:", form));
    transactionPropertyCombo = new QComboBox(form);
    {
        PropertyManager pm;
        auto props = pm.getAllProperties();
        for (auto &p : props) {
            transactionPropertyCombo->addItem(QString("%1 (ID:%2)").arg(p.name).arg(p.id), p.id);
        }
    }
    layout->addWidget(transactionPropertyCombo);

    // 2) Выбор арендатора (может быть пусто при продаже)
    layout->addWidget(new QLabel("Арендатор (опционально):", form));
    transactionTenantCombo = new QComboBox(form);
    transactionTenantCombo->addItem("— нет —", QVariant(0));
    {
        TenantManager tm;
        auto tenants = tm.getAllTenants();
        for (auto &t : tenants) {
            transactionTenantCombo->addItem(QString("%1 (ID:%2)").arg(t.name).arg(t.id), t.id);
        }
    }
    layout->addWidget(transactionTenantCombo);

    // 3) Тип сделки
    layout->addWidget(new QLabel("Тип сделки:", form));
    transactionTypeCombo = new QComboBox(form);
    transactionTypeCombo->addItems({"Продажа", "Аренда"});
    layout->addWidget(transactionTypeCombo);

    // 4) Дата
    layout->addWidget(new QLabel("Дата сделки:", form));
    transactionDateEdit = new QDateEdit(QDate::currentDate(), form);
    transactionDateEdit->setCalendarPopup(true);
    layout->addWidget(transactionDateEdit);

    // 5) Сумма
    layout->addWidget(new QLabel("Сумма:", form));
    transactionAmountEdit = new QLineEdit(form);
    layout->addWidget(transactionAmountEdit);

    // 6) Кнопка «Сохранить»
    QPushButton *saveBtn = new QPushButton("Сохранить сделку", form);
    layout->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked,
            this, &MainWindow::on_addTransactionButton_clicked);

    form->setLayout(layout);
    form->resize(300, 400);
    form->show();
}

void MainWindow::on_addTransactionButton_clicked() {
    // Собираем данные
    Transaction t;
    t.propertyId = transactionPropertyCombo->currentData().toInt();
    t.tenantId   = transactionTenantCombo->currentData().toInt();
    t.type       = transactionTypeCombo->currentText();
    t.date       = transactionDateEdit->date();
    t.amount     = transactionAmountEdit->text().toDouble();

    // Валидация
    if (t.propertyId <= 0) {
        QMessageBox::warning(nullptr, "Ошибка", "Выберите недвижимость!");
        return;
    }
    if (t.amount <= 0.0) {
        QMessageBox::warning(nullptr, "Ошибка", "Введите корректную сумму сделки!");
        return;
    }

    // Сохраняем
    TransactionManager mgr;
    if (!mgr.addTransaction(t)) {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось сохранить сделку.");
        return;
    }

    QMessageBox::information(nullptr, "Готово", "Сделка успешно сохранена.");
}

