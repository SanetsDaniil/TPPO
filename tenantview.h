#ifndef TENANTVIEW_H
#define TENANTVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include "tenantmanager.h"

class TenantView : public QWidget {
    Q_OBJECT
public:
    explicit TenantView(QWidget *parent = nullptr);
    void loadTenants();
private:
    QTableWidget *table;
    QPushButton *deleteButton;
    QLineEdit *nameFilterEdit;
    QComboBox *typeFilterCombo;

private slots:
    void applyFilters();
    void deleteTenant();
};

#endif // TENANTVIEW_H
