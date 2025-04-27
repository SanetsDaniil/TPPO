#include "tenantmanager.h"

TenantManager::TenantManager() {
    db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS tenants ("
               "id INTEGER PRIMARY KEY, "
               "type TEXT NOT NULL, "   // добавили
               "name TEXT NOT NULL, "
               "property_id INTEGER, "
               "month_cost REAL, "
               "lease_start DATE, "
               "lease_end DATE)");
}

bool TenantManager::addTenant(const Tenant& tenant) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO tenants (type, name, property_id, month_cost, lease_start, lease_end) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(tenant.type);  // Добавили тип!
    query.addBindValue(tenant.name);
    query.addBindValue(tenant.propertyId);
    query.addBindValue(tenant.monthCost);
    query.addBindValue(tenant.leaseStart);
    query.addBindValue(tenant.leaseEnd);
    return query.exec();
}

QVector<Tenant> TenantManager::getAllTenants() {
    QVector<Tenant> tenants;
    QSqlQuery query(db);
    query.exec("SELECT * FROM tenants");
    while (query.next()) {
        Tenant t;
        t.id = query.value(0).toInt();
        t.type = query.value(1).toString(); // тип
        t.name = query.value(2).toString();
        t.propertyId = query.value(3).toInt();
        t.monthCost = query.value(4).toDouble();
        t.leaseStart = query.value(5).toDate();
        t.leaseEnd = query.value(6).toDate();
        tenants.append(t);
    }
    return tenants;
}

QVector<Tenant> TenantManager::searchTenants(const QString& searchText) {
    QVector<Tenant> tenants;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM tenants WHERE name LIKE ?");
    query.addBindValue("%" + searchText + "%");
    query.exec();
    while (query.next()) {
        Tenant t;
        t.id = query.value(0).toInt();
        t.name = query.value(1).toString();
        t.propertyId = query.value(2).toInt();
        t.monthCost = query.value(3).toDouble();
        t.leaseStart = query.value(4).toDate();
        t.leaseEnd = query.value(5).toDate();
        tenants.append(t);
    }
    return tenants;
}
