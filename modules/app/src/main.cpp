#include <QApplication>
#include <QString>
#include <QWidget>
#include <teks/teks.hpp>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle(QString::fromStdString(teks::test()));
    window.resize(800, 600);
    window.show();

    return app.exec();
}
