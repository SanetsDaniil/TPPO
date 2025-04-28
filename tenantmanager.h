#ifndef TENANTMANAGER_H
#define TENANTMANAGER_H

#include <QString>
#include <QDate>
#include <QVector>
#include <QSqlQuery>
#include "database.h"

struct Tenant {
    int id;
    QString type;        // Арендатор / Покупатель
    QString name;
    QString email;
    QDate   birthDate;
    QString phone;
};

class TenantManager {
public:
    TenantManager();
    int addTenant(const Tenant& tenant);
    QVector<Tenant> getAllTenants();
    QVector<Tenant> searchTenants(const QString& searchText);

private:
    QSqlDatabase db;
};

#endif // TENANTMANAGER_H
