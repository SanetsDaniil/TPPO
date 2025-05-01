#ifndef TENANTVIEW_H
#define TENANTVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include "tenantmanager.h"
#include "database.h"

class TenantView : public QWidget {
    Q_OBJECT
public:
    explicit TenantView(QWidget *parent = nullptr);
    void loadTenants();
    static constexpr int WARNING_DAYS = 30; // порог подсветки

private slots:
    void applyFilters();
    void deleteTenant();

private:
    QTableWidget *table;
    QPushButton *deleteButton;
    QLineEdit *nameFilterEdit;
    QComboBox *typeFilterCombo;

};

#endif // TENANTVIEW_H
