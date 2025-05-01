#include <QApplication>
#include "mainwindow.h"
#include "sampledata.h"
#include "database.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    if (!DatabaseManager::instance().openDatabase()) {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось подключиться к базе данных!");
        return 1;
    }

    populateSampleData();

    PropertyManager().releaseExpiredLeases();
    populateSampleData();

    MainWindow mainWindow;

    mainWindow.show();

    return app.exec();
}
