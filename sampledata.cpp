#include "sampledata.h"
#include "database.h"
#include "propertymanager.h"
#include "tenantmanager.h"
#include "transactionmanager.h"
#include <QDate>
#include <QDebug>

void populateSampleData() {
    auto &db = DatabaseManager::instance().getDatabase();

    QSqlQuery q(db);
    // Если уже есть данные — не заполняем
    q.exec("SELECT COUNT(*) FROM properties");
    if (q.next() && q.value(0).toInt() > 0) {
        qDebug() << "Sample data already exists, skipping.";
        return;
    }

    PropertyManager pm;
    TenantManager tm;
    TransactionManager xmgr;

    // 1) Генерируем 50 объектов недвижимости
    QStringList types   = {"Квартира","Дом","Офис","Склад"};
    QStringList statuses= {"Свободно","Занято","Продано"};
    for (int i = 1; i <= 50; ++i) {
        Property p;
        p.name     = QString("Test Property #%1").arg(i);
        p.type     = types.at(i % types.size());
        p.location = QString("Location %1").arg(i);
        p.status   = statuses.at(i % statuses.size());
        // цена от 10000 до 60000
        p.price    = 10000 + (i * 500);
        if (!pm.addProperty(p))
            qDebug() << "Failed to add property" << p.name;
    }

    // 2) Генерируем 50 арендаторов / покупателей
    for (int i = 1; i <= 50; ++i) {
        Tenant t;
        t.type      = (i % 5 == 0) ? "Покупатель" : "Арендатор"; // каждый 5-й — покупатель
        t.name      = QString("%1 %2").arg((t.type == "Арендатор") ? "Арендатор" : "Покупатель").arg(i);
        t.email     = QString("user%1@example.com").arg(i);
        t.birthDate = QDate::currentDate().addYears(-20 - (i % 30));
        t.phone     = QString("+7 900 %1").arg(1000000 + i);
        int newId = tm.addTenant(t);
        if (newId < 0)
            qDebug() << "Failed to add tenant" << t.name;
    }

    // 3) Генерируем 50 сделок: чередуем аренду и продажу
    QVector<Property> props = pm.getAllProperties();
    QVector<Tenant>  tenants= tm.getAllTenants();
    int pCount = props.size();
    int tCount = tenants.size();
    for (int i = 0; i < 50; ++i) {
        const auto &p = props.at(i % pCount);
        const auto &t = tenants.at(i % tCount);

        Transaction tx;
        tx.propertyId   = p.id;
        tx.tenantId     = (i % 2 == 0 ? t.id : 0); // чётные — аренда с арендатором, нечётные — продажа
        tx.type         = (i % 2 == 0 ? "Аренда" : "Продажа");
        tx.date         = QDate::currentDate().addDays(-i);
        tx.propertyName = p.name;
        tx.tenantName   = (tx.tenantId ? t.name : QString("—"));

        if (tx.type == "Аренда") {
            // аренда на i%12+1 месяцев
            tx.leaseStart = tx.date;
            tx.leaseEnd   = tx.date.addMonths((i % 12) + 1);
            int months    = tx.leaseStart.daysTo(tx.leaseEnd) / 30;
            if (months < 1) months = 1;
            tx.amount     = p.price * months;
        } else {
            // продажа — 12×месячная ставка
            tx.amount     = p.price * 12;
        }

        if (!xmgr.addTransaction(tx))
            qDebug() << "Failed to add transaction for property" << p.id;
    }

    qDebug() << "Sample data: 50 properties, 50 tenants, 50 transactions added.";
}
