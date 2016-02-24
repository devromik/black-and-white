#ifndef BW_TREE_ITEM_GRAPHICS_H
#define BW_TREE_ITEM_GRAPHICS_H

#include <QList>
#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include "Tree.h"
#include "BWTreeColorer.h"
#include "TreeView.h"

namespace bw {

template<typename NodeType>
class EdgeGraphics;

/**
 * Графическое представление узла дерева.
 */
template<typename NodeType>
class NodeGraphics : public QGraphicsItem {

public:

    typedef Tree<NodeType> TreeType;
    typedef BWTreeColorer<NodeType> ColorerType;
    typedef EdgeGraphics<NodeType> EdgeGraphicsType;

public:

    enum State {
        CREATE = 1,
        CREATED = 2,
        SELECTED = 4
    };

public:

    static const double RADIUS;
    static const double DIAMETER;

    static const double CENTER_X;
    static const double CENTER_Y;

    static const double Z_VALUE;

public:

    /**
     * Параметры отображения.
     */
    class DisplayOp {

    public:

        DisplayOp() {
            byDefault();
        }

        void setBackgroundBrush(const QBrush& newBackgroundBrush) {
            backgroundBrush = newBackgroundBrush;
        }

        void setBorderPen(const QPen& newBorderPen) {
            borderPen = newBorderPen;
        }

        void byDefault() {
            // Фон.
            setBackgroundBrush(QColor(253, 214, 55));

            // Рамка.
            setBorderPen(QPen(QColor(91, 91, 91), 2.7));
        }

    public:

        // Фон.
        QBrush backgroundBrush;

        // Рамка.
        QPen borderPen;
    };

public:

    NodeGraphics(NodeType* newNode, const int newState = 0) : node(newNode), state(newState) {
        init();
    }

    NodeType* getNode() const {
        return node;
    }

    QRectF boundingRect() const {
        double halfPenWidth = displayOp.borderPen.widthF() / 2.0;
        return QRectF(-RADIUS - halfPenWidth, -RADIUS - halfPenWidth, DIAMETER  + halfPenWidth, DIAMETER + halfPenWidth);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {
        painter->save();

        if (state & (CREATE | SELECTED)) {
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(displayOp.borderPen);
            QBrush b(displayOp.backgroundBrush);
            b.setColor(QColor(198, 217, 255));
            painter->setBrush(b);
            painter->drawEllipse(-(RADIUS), -(RADIUS), DIAMETER, DIAMETER);
        }
        else {
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(displayOp.borderPen);
            painter->setBrush(displayOp.backgroundBrush);
            painter->drawEllipse(-RADIUS, -RADIUS, DIAMETER, DIAMETER);
        }

        painter->restore();
    }

    void addEdgeGraphics(EdgeGraphicsType* newEdgeGraphics) {
        edgeGraphics.append(newEdgeGraphics);
        updateEdgeGraphics();
    }

    void removeEdgeGraphics(EdgeGraphicsType* graphics) {
        edgeGraphics.removeOne(graphics);
        updateEdgeGraphics();
    }

    EdgeGraphicsType* getEdgeGraphics(const int i) {
        return edgeGraphics[i];
    }

    int getEdgeGraphicsCount() const {
        return edgeGraphics.size();
    }

    void move(const double dx, const double dy) {
        moveBy(dx, dy);
        updateEdgeGraphics();
    }

    QPointF getCenter() const {
        static QPointF center(CENTER_X, CENTER_Y);
        return center;
    }

    void setState(const int newState) {
        state = newState;
        update();
    }

    int getState() const {
        return state;
    }

    void setBackgroundBrush(const QBrush& newBackgroundBrush) {
        displayOp.setBackgroundBrush(newBackgroundBrush);
    }

    void setBorderPen(const QPen& newBorderPen) {
        displayOp.setBorderPen(newBorderPen);
    }

    void setDefaultDisplayOp() {
        displayOp.byDefault();
    }

protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) {
        if (change == ItemSceneChange) {
            updateEdgeGraphics();
        }

        return QGraphicsItem::itemChange(change, value);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
        updateEdgeGraphics();
        QGraphicsItem::mouseMoveEvent(event);
    }

private:

    void init() {
        setFlag(QGraphicsItem::ItemIsSelectable, false);
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setCursor(QCursor(Qt::PointingHandCursor));
        setZValue(Z_VALUE);
    }

    void updateEdgeGraphics() {
        typename QList<EdgeGraphicsType*>::Iterator edgeGraphicsIter = edgeGraphics.begin();

        while (edgeGraphicsIter != edgeGraphics.end()) {
            (*edgeGraphicsIter)->nodesUpdated();
            (*edgeGraphicsIter)->repaint();
            ++edgeGraphicsIter;
        }
    }

private:

    // Узел.
    NodeType* node;

    // Графические представления рёбер, инцидентных node.
    QList<EdgeGraphics<NodeType>*> edgeGraphics;

    // Состояние.
    int state;

    // Параметры отображения.
    DisplayOp displayOp;
};

/**
 * Графическое представление ребра дерева.
 */
template<typename NodeType>
class EdgeGraphics : public QGraphicsItem {

public:

    /**
     * Соответствующий тип графического представления узла дерева.
     */
    typedef NodeGraphics<NodeType> NodeGraphicsType;

public:

    static constexpr double Z_VALUE = /*NodeGraphicsType::Z_VALUE - */1.0;

public:

    /**
     * Параметры отображения.
     */
    class DisplayOp {

    public:

        DisplayOp() {
            byDefault();
        }

        void setSegmentPen(const QPen& newSegmentPen) {
            segmentPen = newSegmentPen;
        }

        void byDefault() {
            // Отрезок, представляющий ребро.
            setSegmentPen(QPen(QColor(89, 89, 89, 180), 3.0));
        }

    public:

        // Отрезок, представляющий ребро.
        QPen segmentPen;
    };

public:

    EdgeGraphics(NodeGraphicsType* newNode1, NodeGraphicsType* newNode2) : node1(newNode1), node2(newNode2) {
        setZValue(Z_VALUE);
        nodesUpdated();
    }

    QRectF boundingRect() const {
        double leftX = qMin(node1Center.x(), node2Center.x());
        double topY = qMin(node1Center.y(), node2Center.y());

        double halfSegmentPenWidth = displayOp.segmentPen.widthF() / 2.0;

        return QRectF(
            leftX,
            topY,
            qAbs(node1Center.x() - node2Center.x()),
            qAbs(node1Center.y() - node2Center.y())).adjusted(-halfSegmentPenWidth, -halfSegmentPenWidth, halfSegmentPenWidth, halfSegmentPenWidth);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(displayOp.segmentPen);
        painter->drawLine(node1Center, node2Center);
        painter->restore();
    }

    void repaint() {
        update();
    }

    void nodesUpdated() {
        prepareGeometryChange();
        calcNodeCenters();
    }

    void setSegmentPen(const QPen& newSegmentPen) {
        displayOp.setSegmentPen(newSegmentPen);
    }

    void setDefaultDisplayOp() {
        displayOp.byDefault();
    }

private:

    void calcNodeCenters() {
        node1Center = mapFromItem(node1, node1->getCenter());
        node2Center = mapFromItem(node2, node2->getCenter());
    }

private:

    // Узлы и их центры.
    NodeGraphicsType* node1;
    QPointF node1Center;

    NodeGraphicsType* node2;
    QPointF node2Center;

    // Параметры отображения.
    DisplayOp displayOp;
};

template<typename NodeType> const double NodeGraphics<NodeType>::RADIUS = 30.0;
template<typename NodeType> const double NodeGraphics<NodeType>::DIAMETER = 60.0;

template<typename NodeType> const double NodeGraphics<NodeType>::CENTER_X = 0.0;
template<typename NodeType> const double NodeGraphics<NodeType>::CENTER_Y = 0.0;

template<typename NodeType> const double NodeGraphics<NodeType>::Z_VALUE = 3.0;

} // namespace bw.

#endif
