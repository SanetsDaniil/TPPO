#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "propertyview.h"
#include "propertymanager.h"
#include "tenantmanager.h"
#include "tenantview.h"
#include "database.h"
#include "transactionview.h"
#include "transactionmanager.h"


#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
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
    QLineEdit *searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("Поиск недвижимости...");
    connect(searchLineEdit, &QLineEdit::textChanged, view, &PropertyView::filterProperties);
    view->layout()->addWidget(searchLineEdit);  // Добавляем поле для поиска
    setCentralWidget(view);
}

void MainWindow::showPropertyForm()
{
    QWidget *formWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(formWidget);

    editPropertyComboBox = new QComboBox(this);
    layout->addWidget(editPropertyComboBox);
    connect(editPropertyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::fillPropertyFieldsFromSelected);
    QLabel *label1 = new QLabel("Название недвижимости:");
    propertyNameLineEdit = new QLineEdit();
    QLabel *label2 = new QLabel("Тип недвижимости:");
    propertyTypeComboBox = new QComboBox();
    propertyTypeComboBox->addItems({"Квартира", "Дом", "Офис", "Склад"});
    QLabel *label3 = new QLabel("Расположение:");
    propertyLocationLineEdit = new QLineEdit();
    QLabel *label4 = new QLabel("Статус:");
    propertyStatusComboBox = new QComboBox();
    propertyStatusComboBox->addItems({"Свободно", "Занято", "Продано"});
    QLabel *label5 = new QLabel("Цена:");
    propertyPriceLineEdit = new QLineEdit();
    QPushButton *addButton = new QPushButton("Добавить недвижимость");

    layout->addWidget(label1);
    layout->addWidget(propertyNameLineEdit);
    layout->addWidget(label2);
    layout->addWidget(propertyTypeComboBox);
    layout->addWidget(label3);
    layout->addWidget(propertyLocationLineEdit);
    layout->addWidget(label4);
    layout->addWidget(propertyStatusComboBox);
    layout->addWidget(label5);
    layout->addWidget(propertyPriceLineEdit);
    layout->addWidget(addButton);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::on_addPropertyButton_clicked);

    formWidget->setLayout(layout);
    setCentralWidget(formWidget);
    loadPropertiesToComboBox();
}

void MainWindow::showTenantForm()
{
    QWidget *formWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(formWidget);

    QLabel *label6 = new QLabel("Имя арендатора:");
    tenantNameLineEdit = new QLineEdit();
    QLabel *label7 = new QLabel("ID недвижимости:");
    tenantPropertyIdLineEdit = new QLineEdit();
    QLabel *label8 = new QLabel("Стоимость аренды");
    monthCost = new QLineEdit();
    QLabel *label9 = new QLabel("Дата начала аренды:");
    leaseStartDateEdit = new QDateEdit();
    leaseStartDateEdit->setCalendarPopup(true);
    QLabel *label10 = new QLabel("Дата окончания аренды:");
    leaseEndDateEdit = new QDateEdit();
    leaseEndDateEdit->setCalendarPopup(true);
    QPushButton *addTenantButton = new QPushButton("Добавить арендатора");

    layout->addWidget(label6);
    layout->addWidget(tenantNameLineEdit);
    layout->addWidget(label7);
    layout->addWidget(tenantPropertyIdLineEdit);
    layout->addWidget(label8);
    layout->addWidget(monthCost);
    layout->addWidget(label9);
    layout->addWidget(leaseStartDateEdit);
    layout->addWidget(label10);
    layout->addWidget(leaseEndDateEdit);
    layout->addWidget(addTenantButton);

    connect(addTenantButton, &QPushButton::clicked, this, &MainWindow::on_addTenantButton_clicked);

    formWidget->setLayout(layout);
    setCentralWidget(formWidget);

}

void MainWindow::showTenantList()
{
    TenantView *view = new TenantView(this);
    setCentralWidget(view);
}


void MainWindow::on_addPropertyButton_clicked() {
    Property property;
    property.name = propertyNameLineEdit->text();
    property.type = propertyTypeComboBox->currentText();
    property.location = propertyLocationLineEdit->text();
    property.status = propertyStatusComboBox->currentText();
    property.price = propertyPriceLineEdit->text().toDouble();

    PropertyManager manager;

    int selectedIndex = editPropertyComboBox->currentIndex();
    if (selectedIndex == 0) {
        // Добавляем новую
        if (manager.addProperty(property)) {
            QMessageBox::information(this, "Успех", "Недвижимость добавлена.");
        }
    } else {
        // Обновляем существующую
        property.id = currentProperties[selectedIndex - 1].id;
        if (manager.removeProperty(property.id) && manager.addProperty(property)) {
            QMessageBox::information(this, "Успех", "Недвижимость обновлена.");
        }
    }

    loadPropertiesToComboBox();
}

void MainWindow::on_addTenantButton_clicked()
{
    Tenant tenant;
    tenant.name = tenantNameLineEdit->text();
    tenant.propertyId = tenantPropertyIdLineEdit->text().toInt();
    tenant.monthCost = monthCost->text().toDouble();
    tenant.leaseStart = leaseStartDateEdit->date();
    tenant.leaseEnd = leaseEndDateEdit->date();

    TenantManager manager;
    if (manager.addTenant(tenant)) {
        QMessageBox::information(this, "Успех", "Арендатор добавлен!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить арендатора.");
    }
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

