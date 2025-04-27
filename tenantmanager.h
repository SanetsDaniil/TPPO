#ifndef TENANTMANAGER_H
#define TENANTMANAGER_H

#include <QString>
#include <QDate>
#include <QVector>
#include <QSqlQuery>
#include "database.h"

struct Tenant {
    int id;
    QString name;
    int propertyId;
    double monthCost;
    QDate leaseStart;
    QDate leaseEnd;
    QString type;
};

class TenantManager {
public:
    TenantManager();
    bool addTenant(const Tenant& tenant);
    QVector<Tenant> getAllTenants();
    QVector<Tenant> searchTenants(const QString& searchText);

private:
    QSqlDatabase db;
};

#endif // TENANTMANAGER_H
