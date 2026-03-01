#include "MainWindow.hpp"
#include <teks/editor/DocumentView.hpp>
#include <QVBoxLayout>

namespace teks::app {
    MainWindow::MainWindow()
        : documentView_(new editor::DocumentView(this))
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(documentView_);

        setWindowTitle(QString("Teks"));
    }
} // namespace teks::app
