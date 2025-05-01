#include "propertymanager.h"
#include "database.h"
#include <QSqlQuery>

PropertyManager::PropertyManager() {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.exec("CREATE TABLE IF NOT EXISTS properties (id INTEGER PRIMARY KEY, name TEXT, type TEXT, location TEXT, status TEXT, price REAL)");
}

bool PropertyManager::addProperty(const Property& property) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO properties (name, type, location, status, price) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(property.name);
    query.addBindValue(property.type);
    query.addBindValue(property.location);
    query.addBindValue(property.status);
    query.addBindValue(property.price);
    return query.exec();
}

bool PropertyManager::removeProperty(int propertyId) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM properties WHERE id = ?");
    query.addBindValue(propertyId);
    return query.exec();
}

QVector<Property> PropertyManager::getAllProperties() {
    QVector<Property> properties;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.exec("SELECT * FROM properties");
    while (query.next()) {
        Property p;
        p.id = query.value(0).toInt();
        p.name = query.value(1).toString();
        p.type = query.value(2).toString();
        p.location = query.value(3).toString();
        p.status = query.value(4).toString();
        p.price = query.value(5).toDouble();
        properties.append(p);
    }
    return properties;
}

bool PropertyManager::updateStatus(int propertyId, const QString& newStatus) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE properties SET status = ? WHERE id = ?");
    query.addBindValue(newStatus);
    query.addBindValue(propertyId);
    return query.exec();
}

QVector<Property> PropertyManager::getFreeProperties() {
    QVector<Property> out;
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT * FROM properties WHERE status = 'Свободно'");
    query.exec();
    while (query.next()) {
        Property p;
        p.id       = query.value("id").toInt();
        p.name     = query.value("name").toString();
        p.type     = query.value("type").toString();
        p.location = query.value("location").toString();
        p.status   = query.value("status").toString();
        p.price    = query.value("price").toDouble();
        out.append(p);
    }
    return out;
}

void PropertyManager::releaseExpiredLeases() {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    // Для каждого property_id из transactions, если тип аренда и lease_end < сегодня
    query.prepare(R"(
        UPDATE properties
        SET status = 'Свободно'
        WHERE id IN (
          SELECT property_id FROM transactions
          WHERE type = 'Аренда' AND DATE(lease_end) < DATE('now')
        )
    )");
    query.exec();
}


