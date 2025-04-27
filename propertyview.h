#ifndef PROPERTYVIEW_H
#define PROPERTYVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSqlQuery>
#include <QComboBox>
#include <QDebug>
#include <QSqlError>
#include "propertymanager.h"

class PropertyView : public QWidget {
    Q_OBJECT

public:
    explicit PropertyView(QWidget *parent = nullptr);
    void loadProperties();  // Загрузка свойств
    void deleteProperty();  // Удаление недвижимости

private slots:
    void applyFilters();

private:
    QTableWidget *table;
    QPushButton *deleteButton;
    QLineEdit *searchLineEdit;
    QLineEdit    *locationFilterEdit;      // Фильтр по локации
    QComboBox    *typeFilterCombo;         // Фильтр по типу
    QComboBox    *statusFilterCombo;       // Фильтр по статусу

    bool deletePropertyFromDatabase(int propertyId);// Поле для поиска
};

#endif // PROPERTYVIEW_H
