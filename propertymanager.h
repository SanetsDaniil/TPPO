#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include <QString>
#include <QVector>

struct Property {
    int id;
    QString name;
    QString type;
    QString location;
    QString status;
    double price;
};

class PropertyManager {
public:
    PropertyManager();
    bool addProperty(const Property& property);
    bool removeProperty(int propertyId);  // Функция удаления недвижимости
    QVector<Property> getAllProperties();
};

#endif // PROPERTYMANAGER_H
