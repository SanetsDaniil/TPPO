#ifndef TENANTVIEW_H
#define TENANTVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "tenantmanager.h"

class TenantView : public QWidget {
    Q_OBJECT
public:
    explicit TenantView(QWidget *parent = nullptr);
    void loadTenants();
private:
    QTableWidget *table;
};

#endif // TENANTVIEW_H
