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
    /// Устанавливает новый статус у объекта по его ID
    bool updateStatus(int propertyId, const QString& newStatus);
    /// Возвращает только те объекты, у которых status == "Свободно"
    QVector<Property> getFreeProperties();
    /// Сбрасывает статус на "Свободно" для всех объектов, чей последний lease_end уже прошёл
    void releaseExpiredLeases();
};

#endif // PROPERTYMANAGER_H
