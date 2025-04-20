#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "propertyview.h"
#include "propertymanager.h"
#include "tenantmanager.h"
#include "tenantview.h"
#include "database.h"

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

    toolbar->addAction(viewPropertiesAction);
    toolbar->addAction(viewTenantAction);
    toolbar->addAction(addTenantAction);
    toolbar->addAction(addPropertyAction);

    connect(viewPropertiesAction, &QAction::triggered, this, &MainWindow::showPropertyList);
    connect(viewTenantAction,&QAction::triggered, this, &MainWindow::showTenantList);
    connect(addTenantAction, &QAction::triggered, this, &MainWindow::showTenantForm);
    connect(addPropertyAction, &QAction::triggered, this, &MainWindow::showPropertyForm);

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

}

void MainWindow::showTenantForm()
{
    QWidget *formWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(formWidget);

    QLabel *label6 = new QLabel("Имя арендатора:");
    tenantNameLineEdit = new QLineEdit();
    QLabel *label7 = new QLabel("ID недвижимости:");
    tenantPropertyIdLineEdit = new QLineEdit();
    QLabel *label8 = new QLabel("Дата начала аренды:");
    leaseStartDateEdit = new QDateEdit();
    leaseStartDateEdit->setCalendarPopup(true);
    QLabel *label9 = new QLabel("Дата окончания аренды:");
    leaseEndDateEdit = new QDateEdit();
    leaseEndDateEdit->setCalendarPopup(true);
    QPushButton *addTenantButton = new QPushButton("Добавить арендатора");

    layout->addWidget(label6);
    layout->addWidget(tenantNameLineEdit);
    layout->addWidget(label7);
    layout->addWidget(tenantPropertyIdLineEdit);
    layout->addWidget(label8);
    layout->addWidget(leaseStartDateEdit);
    layout->addWidget(label9);
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


void MainWindow::on_addPropertyButton_clicked()
{
    Property property;
    property.name = propertyNameLineEdit->text();
    property.type = propertyTypeComboBox->currentText();
    property.location = propertyLocationLineEdit->text();
    property.status = propertyStatusComboBox->currentText();
    property.price = propertyPriceLineEdit->text().toDouble();

    PropertyManager manager;
    if (manager.addProperty(property)) {
        QMessageBox::information(this, "Успех", "Недвижимость добавлена!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить недвижимость.");
    }

}

void MainWindow::on_addTenantButton_clicked()
{
    Tenant tenant;
    tenant.name = tenantNameLineEdit->text();
    tenant.propertyId = tenantPropertyIdLineEdit->text().toInt();
    tenant.leaseStart = leaseStartDateEdit->date();
    tenant.leaseEnd = leaseEndDateEdit->date();

    TenantManager manager;
    if (manager.addTenant(tenant)) {
        QMessageBox::information(this, "Успех", "Арендатор добавлен!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить арендатора.");
    }

}

