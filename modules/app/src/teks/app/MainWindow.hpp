#pragma once

#include <teks/editor/DocumentView.hpp>
#include <QWidget>

namespace teks::app {
    struct MainWindow : public QWidget {
        MainWindow();

    private:
        editor::DocumentView* documentView_;
    };
} // namespace teks::app
