#include "propertyview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QHeaderView>

PropertyView::PropertyView(QWidget *parent) : QWidget(parent) {
    // Основная вертикальная компоновка
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 1) Полоса фильтров (горизонтальный Layout)
    QHBoxLayout *filterLayout = new QHBoxLayout;

    // Тип недвижимости
    typeFilterCombo = new QComboBox(this);
    typeFilterCombo->addItems({"Все", "Квартира", "Дом", "Офис", "Склад"});
    filterLayout->addWidget(typeFilterCombo);

    // Статус
    statusFilterCombo = new QComboBox(this);
    statusFilterCombo->addItems({"Все", "Свободно", "Занято", "Продано"});
    filterLayout->addWidget(statusFilterCombo);

    // Локация
    locationFilterEdit = new QLineEdit(this);
    locationFilterEdit->setPlaceholderText("Фильтр по локации...");
    filterLayout->addWidget(locationFilterEdit);

    // Добавляем полосу фильтров в общий лэйаут
    mainLayout->addLayout(filterLayout);

    // 2) Таблица
    table = new QTableWidget(this);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(table);

    // 3) Кнопка удаления
    deleteButton = new QPushButton("Удалить отмеченные", this);
    mainLayout->addWidget(deleteButton);

    setLayout(mainLayout);

    // Загрузить данные
    loadProperties();

    // Подключаем сигналы фильтров к слоту
    connect(typeFilterCombo,   &QComboBox::currentTextChanged, this, &PropertyView::applyFilters);
    connect(statusFilterCombo, &QComboBox::currentTextChanged, this, &PropertyView::applyFilters);
    connect(locationFilterEdit,&QLineEdit  ::textChanged,       this, &PropertyView::applyFilters);

    // Кнопка удаления
    connect(deleteButton, &QPushButton::clicked, this, &PropertyView::deleteProperty);
}

void PropertyView::loadProperties() {
    PropertyManager manager;
    QVector<Property> properties = manager.getAllProperties();

    table->setRowCount(properties.size());
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"ID", "Название", "Тип", "Локация", "Статус"});

    for (int i = 0; i < properties.size(); ++i) {
        const auto &p = properties[i];
        table->setItem(i, 0, new QTableWidgetItem(QString::number(p.id)));
        table->setItem(i, 1, new QTableWidgetItem(p.name));
        table->setItem(i, 2, new QTableWidgetItem(p.type));
        table->setItem(i, 3, new QTableWidgetItem(p.location));
        table->setItem(i, 4, new QTableWidgetItem(p.status));
    }

    table->horizontalHeader()->setStretchLastSection(true);
    table->resizeColumnsToContents();

    // После загрузки данных применяем текущие фильтры
    applyFilters();
}

void PropertyView::applyFilters() {
    QString typeFilter   = typeFilterCombo->currentText();
    QString statusFilter = statusFilterCombo->currentText();
    QString locFilter    = locationFilterEdit->text().trimmed();

    for (int row = 0; row < table->rowCount(); ++row) {
        bool visible = true;

        // Фильтр по типу
        if (typeFilter != "Все") {
            QString cellType = table->item(row, 2)->text();
            if (cellType != typeFilter) visible = false;
        }

        // Фильтр по статусу
        if (visible && statusFilter != "Все") {
            QString cellStatus = table->item(row, 4)->text();
            if (cellStatus != statusFilter) visible = false;
        }

        // Фильтр по локации (подстрока, регистр игнорируется)
        if (visible && !locFilter.isEmpty()) {
            QString cellLoc = table->item(row, 3)->text();
            if (!cellLoc.contains(locFilter, Qt::CaseInsensitive)) visible = false;
        }

        table->setRowHidden(row, !visible);
    }
}

// Метод удаления остается без изменений:
void PropertyView::deleteProperty() {
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите строку для удаления");
        return;
    }
    int propertyId = table->item(row, 0)->text().toInt();
    if (deletePropertyFromDatabase(propertyId)) {
        table->removeRow(row);
        QMessageBox::information(this, "Готово", "Недвижимость удалена");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить недвижимость");
    }
}


bool PropertyView::deletePropertyFromDatabase(int propertyId) {
    QSqlQuery query;

    // Создаем SQL-запрос для удаления недвижимости по ID
    query.prepare("DELETE FROM properties WHERE id = :id");
    query.bindValue(":id", propertyId);

    if (query.exec()) {
        return true;
    } else {
        // Если ошибка, выводим сообщение
        qDebug() << "Ошибка при удалении записи из базы данных: " << query.lastError().text();
        return false;
    }
}

