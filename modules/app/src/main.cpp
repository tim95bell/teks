#include <QApplication>
#include <QString>
#include <QWidget>
#include <teks/buffer/Buffer.hpp>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    teks::buffer::Buffer buffer;
    teks::buffer::insertStart(buffer, "Hello from Teks");
    const std::string title = teks::buffer::readAllString(buffer);
    window.setWindowTitle(QString::fromStdString(title));
    window.resize(800, 600);
    window.show();

    return app.exec();
}
