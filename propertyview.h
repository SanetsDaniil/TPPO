#ifndef PROPERTYVIEW_H
#define PROPERTYVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include "propertymanager.h"

class PropertyView : public QWidget {
    Q_OBJECT

public:
    explicit PropertyView(QWidget *parent = nullptr);
    void loadProperties();  // Загрузка свойств
    void filterProperties(const QString &filterText);  // Фильтрация свойств
    void deleteProperty();  // Удаление недвижимости

private:
    QTableWidget *table;
    QPushButton *deleteButton;
    QLineEdit *searchLineEdit;

    bool deletePropertyFromDatabase(int propertyId);// Поле для поиска
};

#endif // PROPERTYVIEW_H
