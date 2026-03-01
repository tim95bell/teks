#include <QApplication>
#include <teks/app/MainWindow.hpp>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    teks::app::MainWindow window;
    window.resize(800, 600);
    window.show();

    return app.exec();
}
