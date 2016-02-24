#ifndef BW_MAX_WHITE_MAP_TABLE_VIEW_H
#define BW_MAX_WHITE_MAP_TABLE_VIEW_H

#include <QTableWidget>
#include "MaxWhiteMapView.h"

namespace bw {

class MaxWhiteMapTableView : public MaxWhiteMapView, public QTableWidget {

public:

    MaxWhiteMapTableView(MaxWhiteMap* map = 0);

    void setMap(MaxWhiteMap* newMap) {
        map = newMap;

        if (map == 0) {
            return;
        }

        setColumnCount(map->getNodeCount() + 2);

        for (int b = 0; b <= map->getNodeCount(); ++b) {
            createContentItem(0, b, b);
            createContentItem(1, b, map->get(b));
        }

        disableDecorateLastColumn();
    }

    QWidget* getWidget() {
        return this;
    }

private:

    void init();

    void createContentItem(const int row, const int col, const int value) {
        QTableWidgetItem* newContentItem = new QTableWidgetItem(QString::number(value));
        newContentItem->setFlags(newContentItem->flags() & ~Qt::ItemIsSelectable);
        newContentItem->setFlags(newContentItem->flags() & ~Qt::ItemIsEditable);
        setItem(row, col, newContentItem);
    }

    void createDisableDecorateItem(const int row, const int col) {
        QTableWidgetItem* newDecorateItem = new QTableWidgetItem("");
        newDecorateItem->setBackgroundColor(QColor(182, 182, 182));
        newDecorateItem->setFlags(newDecorateItem->flags() & ~Qt::ItemIsSelectable);
        newDecorateItem->setFlags(newDecorateItem->flags() & ~Qt::ItemIsEditable);
        setItem(row, col, newDecorateItem);
    }

    void disableDecorateLastColumn();

private:

    MaxWhiteMap* map;
};

} // namespace bw.

#endif
