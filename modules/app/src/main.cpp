#include <QApplication>
#include <QString>
#include <QWidget>
#include <teks/buffer/Buffer.hpp>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    teks::buffer::Buffer buffer;
    teks::buffer::insert_start(buffer,  "Hello from Teks");
    const std::string title = teks::buffer::read_all_string(buffer);
    window.setWindowTitle(QString::fromStdString(title));
    window.resize(800, 600);
    window.show();

    return app.exec();
}
