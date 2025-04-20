#include "propertymanager.h"
#include "database.h"
#include <QSqlQuery>

PropertyManager::PropertyManager() {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.exec("CREATE TABLE IF NOT EXISTS properties (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, type TEXT, location TEXT, status TEXT, price REAL)");
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
