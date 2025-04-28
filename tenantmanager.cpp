#include "tenantmanager.h"

TenantManager::TenantManager() {
    db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS tenants ("
               "id INTEGER PRIMARY KEY, "
               "type TEXT NOT NULL, "   // добавили
               "name TEXT NOT NULL, "
               "email TEXT, "
               "birth_date DATE, "
               "phone TEXT)");
}

int TenantManager::addTenant(const Tenant& tenant) {
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO tenants (type, name, email, birth_date, phone) "
        "VALUES (?, ?, ?, ?, ?)"
        );
    query.addBindValue(tenant.type);
    query.addBindValue(tenant.name);
    query.addBindValue(tenant.email);
    query.addBindValue(tenant.birthDate);
    query.addBindValue(tenant.phone);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления арендатора:" << query.lastError().text();
        return -1;
    }
    // Для SQLite lastInsertId() вернёт значение ROWID
    return query.lastInsertId().toInt();
}


QVector<Tenant> TenantManager::getAllTenants() {
    QVector<Tenant> tenants;
    QSqlQuery query(db);
    query.exec("SELECT id, type, name, email, birth_date, phone FROM tenants");
    while (query.next()) {
        Tenant t;
        t.id = query.value("id").toInt();
        t.type = query.value("type").toString();
        t.name = query.value("name").toString();
        t.email = query.value("email").toString();
        t.birthDate = query.value("birth_date").toDate();
        t.phone = query.value("phone").toString();
        tenants.append(t);
    }
    return tenants;
}

QVector<Tenant> TenantManager::searchTenants(const QString& searchText) {
    QVector<Tenant> tenants;
    QSqlQuery query(db);
    query.prepare("SELECT id, type, name, email, birth_date, phone FROM tenants "
                 "WHERE name LIKE ? OR phone LIKE ?");
    query.addBindValue("%" + searchText + "%");
    query.exec();
    while (query.next()) {
        Tenant t;
        t.id = query.value(0).toInt();
        t.name = query.value(1).toString();
        t.email = query.value(3).toString();
        t.birthDate = query.value(4).toDate();
        t.phone = query.value(5).toString();
        tenants.append(t);
    }
    return tenants;
}
