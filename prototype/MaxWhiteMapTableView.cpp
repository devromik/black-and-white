#include "MaxWhiteMapTableView.h"
#include <QHeaderView>

namespace bw {

MaxWhiteMapTableView::MaxWhiteMapTableView(MaxWhiteMap* newMap) {
    init();
    setMap(newMap);
}

void MaxWhiteMapTableView::init() {
    setRowCount(2);
    setColumnCount(0);

    horizontalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setMinimumSectionSize(40);
    horizontalHeader()->setDefaultSectionSize(40);
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    verticalHeader()->setMinimumSectionSize(30);
    verticalHeader()->setDefaultSectionSize(30);
    verticalHeader()->setStretchLastSection(true);
    setMaximumHeight(2.0 * verticalHeader()->defaultSectionSize());
    verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QStringList verticalHeaderLabels;
    verticalHeaderLabels
        << tr("чёрных узлов: ")
        << tr("белых узлов: ");
    setVerticalHeaderLabels(verticalHeaderLabels);

    setSelectionMode(NoSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MaxWhiteMapTableView::disableDecorateLastColumn() {
    createDisableDecorateItem(0, columnCount() - 1);
    createDisableDecorateItem(1, columnCount() - 1);
}

} // namespace bw.

