#include "propertyview.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>

PropertyView::PropertyView(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    table = new QTableWidget(this);
    searchLineEdit = new QLineEdit(this);
    deleteButton = new QPushButton("Удалить недвижимость", this);

    layout->addWidget(searchLineEdit);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(table);
    layout->addWidget(deleteButton);
    setLayout(layout);

    loadProperties();

    connect(searchLineEdit, &QLineEdit::textChanged, this, &PropertyView::filterProperties);
    connect(deleteButton, &QPushButton::clicked, this, &PropertyView::deleteProperty);
}


void PropertyView::loadProperties() {
       PropertyManager manager;
       QVector<Property> properties = manager.getAllProperties();

       table->setRowCount(properties.size());
       table->setColumnCount(5);
       table->setHorizontalHeaderLabels({"ID", "Название", "Тип", "Локация", "Статус"});

       for (int i = 0; i < properties.size(); ++i) {
           table->setItem(i, 0, new QTableWidgetItem(QString::number(properties[i].id)));
           table->setItem(i, 1, new QTableWidgetItem(properties[i].name));
           table->setItem(i, 2, new QTableWidgetItem(properties[i].type));
           table->setItem(i, 3, new QTableWidgetItem(properties[i].location));
           table->setItem(i, 4, new QTableWidgetItem(properties[i].status));
       }
}

void PropertyView::filterProperties(const QString &filterText) {
    for (int i = 0; i < table->rowCount(); ++i) {
        bool matches = false;
        for (int j = 0; j < table->columnCount(); ++j) {
            if (table->item(i, j)->text().contains(filterText, Qt::CaseInsensitive)) {
                matches = true;
                break;
            }
        }
        table->setRowHidden(i, !matches);
    }
}

void PropertyView::deleteProperty() {
    int row = table->currentRow();
        if (row != -1) {
            // Получаем ID недвижимости из таблицы (предположим, что ID в первой колонке)
            int propertyId = table->item(row, 0)->text().toInt();

            // Удаляем запись из базы данных
            if (deletePropertyFromDatabase(propertyId)) {
                // Если удалено, удаляем строку из таблицы
                table->removeRow(row);
                QMessageBox::information(this, "Удаление", "Недвижимость успешно удалена.");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось удалить недвижимость из базы данных.");
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите недвижимость для удаления.");
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

