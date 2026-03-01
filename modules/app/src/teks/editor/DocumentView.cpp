#include "DocumentView.hpp"
#include "Document.hpp"
#include <algorithm>
#include <memory>
#include <filesystem>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>

namespace teks::editor {
    DocumentView::DocumentView(QWidget* parent)
        : QAbstractScrollArea(parent)
    {
        setFocusPolicy(Qt::StrongFocus);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        updateScrollbars();

        // TODO(TB): Replace this development-only bootstrap with real document loading wiring.
        const auto document = Document::openFile(std::filesystem::path(__FILE__));
        if (document.has_value()) {
            setDocument(std::make_shared<Document>(std::move(*document)));
        }
    }

    void DocumentView::paintEvent(QPaintEvent* event) {
        Q_UNUSED(event);

        QPainter p(viewport());
        p.fillRect(viewport()->rect(), palette().base());
        if (!document_) {
            return;
        }

        p.setPen(palette().text().color());
        const QFontMetrics metrics = p.fontMetrics();
        const int lineHeight = metrics.height();
        const int baseline = metrics.ascent();

        const int scrollY = verticalScrollBar()->value();
        const usize firstVisibleLine = static_cast<usize>(scrollY / lineHeight);
        const int yOffsetWithinLine = scrollY % lineHeight;

        int y = baseline - yOffsetWithinLine;
        const int x = 12;

        for (
            usize line = firstVisibleLine;
            line < document_->buffer().lineCount() && y < viewport()->height() + lineHeight;
            ++line
        ) {
            const auto text = buffer::readLine(document_->buffer(), line);
            if (text.has_value()) {
                p.drawText(x, y, QString::fromStdString(*text));
                y += lineHeight;
            }
        }
    }

    void DocumentView::resizeEvent(QResizeEvent* event) {
        QAbstractScrollArea::resizeEvent(event);
        updateScrollbars();
    }

    void DocumentView::scrollContentsBy(int dx, int dy) {
        Q_UNUSED(dx);
        Q_UNUSED(dy);
        viewport()->update();
    }

    void DocumentView::setDocument(std::shared_ptr<Document> document) {
        document_ = std::move(document);

        const QFontMetrics metrics(font());
        const int lineHeight = metrics.height();

        const usize lineCount = document_ ? document_->buffer().lineCount() : 1;
        contentHeight_ = static_cast<int>(lineCount) * lineHeight;

        updateScrollbars();
        viewport()->update();
    }

    void DocumentView::updateScrollbars() {
        const int page = viewport()->height();
        const int maxY = std::max(0, contentHeight_ - page);
        verticalScrollBar()->setRange(0, maxY);
        verticalScrollBar()->setValue(
            std::min(verticalScrollBar()->value(), verticalScrollBar()->maximum())
        );
        verticalScrollBar()->setPageStep(page);
        verticalScrollBar()->setSingleStep(24);
    }
} // namespace teks::editor
