#pragma once

#include <memory>
#include <QAbstractScrollArea>

namespace teks::editor {
    struct Document;

    struct DocumentView final : public QAbstractScrollArea {
        DocumentView(QWidget* parent = nullptr);

    private:
        int contentHeight_{0};
        std::shared_ptr<Document> document_;

        void paintEvent(QPaintEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        void scrollContentsBy(int dx, int dy) override;
        void setDocument(std::shared_ptr<Document> document);
        void updateScrollbars();
    };
} // namespace teks::editor
