#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDateEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <propertymanager.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showPropertyList();
    void showPropertyForm();
    void showTenantForm();
    void showTenantList();
    void on_addPropertyButton_clicked();
    void on_addTenantButton_clicked();
    void showTransactionList();
    void showTransactionForm();               // Шаг 1.2: форма сделки
    void on_addTransactionButton_clicked();   // Слот «Сохранить сделку»
private:
    void setupToolbar();
    QComboBox *editPropertyComboBox;
    QVector<Property> currentProperties;
    void loadPropertiesToComboBox();
    void fillPropertyFieldsFromSelected();
    void setupTransactionAction();

    Ui::MainWindow *ui;

    // Виджеты для формы недвижимости
    QLineEdit *propertyNameLineEdit;
    QComboBox *propertyTypeComboBox;
    QLineEdit *propertyLocationLineEdit;
    QComboBox *propertyStatusComboBox;
    QLineEdit *propertyPriceLineEdit;

    // Виджеты для формы арендатора
    QLineEdit *tenantNameLineEdit;
    QLineEdit *tenantPropertyIdLineEdit;
    QLineEdit *monthCost;
    QDateEdit *leaseStartDateEdit;
    QDateEdit *leaseEndDateEdit;

    //Виджеты формы сделки
    QComboBox   *transactionPropertyCombo;
    QComboBox   *transactionTenantCombo;
    QComboBox   *transactionTypeCombo;
    QDateEdit   *transactionDateEdit;
    QLineEdit   *transactionAmountEdit;
    QDateEdit *transactionLeaseStartEdit;
    QDateEdit *transactionLeaseEndEdit;

};

#endif // MAINWINDOW_H
