#ifndef BW_TREE_VIEW_H
#define BW_TREE_VIEW_H

#include <QMouseEvent>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "TreeModel.h"

namespace bw {

/**
 * Представление дерева.
 */
template<typename NodeType>
class TreeView : public QGraphicsView {

public:

    typedef Tree<NodeType> TreeType;
    typedef TreeModel<NodeType> TreeModelType;
    typedef NodeGraphics<NodeType> NodeGraphicsType;
    typedef EdgeGraphics<NodeType> EdgeGraphicsType;
    typedef BWTreeColoring<NodeType> ColoringType;

public:

    TreeView(TreeModelType* newTreeModel = 0) :
        QGraphicsView((QWidget*)0),
        newParentNodeGraphics(0),
        selectedNodeGraphics(0),
        creatingNodeGraphics(0),
        moveNodeGraphics(0) {

        init();
        setTreeModel(newTreeModel);
    }

    void setTreeModel(TreeModelType* newTreeModel) {
        treeModel = newTreeModel;

        if (treeModel != 0) {
            setScene(treeModel->getScene());
        }
    }

    TreeModelType* getTreeModel() const {
        return treeModel;
    }

protected:

    void keyReleaseEvent(QKeyEvent* event) {
        if (event->key() == Qt::Key_Delete && selectedNodeGraphics != 0 && treeModel->getTree()->getRoot() != selectedNodeGraphics->getNode()) {
            treeModel->removeSubtree(selectedNodeGraphics);
            selectedNodeGraphics = 0;
        }
    }

    void mousePressEvent(QMouseEvent* event) {
        if (event->button() == Qt::LeftButton) {
            if (selectedNodeGraphics != 0) {
                selectedNodeGraphics->setState(selectedNodeGraphics->getState() & ~NodeGraphicsType::SELECTED);
                selectedNodeGraphics = 0;
            }

            QGraphicsItem* itemAtCursorPos = itemAt(event->x(), event->y());

            if (itemAtCursorPos != 0 && isNodeGraphics(itemAtCursorPos)) {
                moveNodeGraphics = (NodeGraphicsType*)itemAtCursorPos;
                moveX = mapToScene(event->pos()).x();
                moveY = mapToScene(event->pos()).y();
                selectedNodeGraphics = (NodeGraphicsType*)itemAtCursorPos;
                selectedNodeGraphics->setState(selectedNodeGraphics->getState() | NodeGraphicsType::SELECTED);
            }
        }
        else if (event->button() == Qt::RightButton) {
            QGraphicsItem* itemAtCursorPos = itemAt(event->x(), event->y());

            if (itemAtCursorPos != 0 && isNodeGraphics(itemAtCursorPos) && treeModel != 0) {
                newParentNodeGraphics = (NodeGraphicsType*)itemAtCursorPos;
                creatingNodeGraphics = treeModel->createChildNode(newParentNodeGraphics, mapToScene(event->pos()));
                creatingNodeGraphics->setState(creatingNodeGraphics->getState() | NodeGraphicsType::CREATE);
            }
        }

        QGraphicsView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent* event) {
        if (creatingNodeGraphics != 0) {
            creatingNodeGraphics->setPos(mapToScene(event->pos()));
        }

        if (moveNodeGraphics != 0) {
            double newMoveX = mapToScene(event->pos()).x();
            double newMoveY = mapToScene(event->pos()).y();
            treeModel->moveSubtree(moveNodeGraphics, newMoveX -moveX, newMoveY - moveY);
            moveX = newMoveX;
            moveY = newMoveY;
        }

        QGraphicsView::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) {
        if (event->button() == Qt::RightButton && creatingNodeGraphics != 0) {
            creatingNodeGraphics->setState((creatingNodeGraphics->getState() & ~NodeGraphicsType::CREATE) | NodeGraphicsType::CREATED);
            newParentNodeGraphics = 0;
            creatingNodeGraphics = 0;
        }

        moveNodeGraphics = 0;
        QGraphicsView::mouseReleaseEvent(event);
    }

private:

    void init() {
        setBackgroundBrush(QColor(245, 245, 245));
    }

    void setNewParentNodeGraphics(NodeGraphicsType* newNewParentGraphics) {
        newParentNodeGraphics = newNewParentGraphics;
    }

    bool isNodeGraphics(QGraphicsItem* graphics) const {
        return dynamic_cast<NodeGraphicsType*>(graphics) != 0;
    }

private:

    // Модель дерева.
    TreeModelType* treeModel;

    // Графическое представление узла, для которого добавляется новый дочерний узел.
    NodeGraphicsType* newParentNodeGraphics;

    NodeGraphicsType* selectedNodeGraphics;
    NodeGraphicsType* creatingNodeGraphics;
    NodeGraphicsType* moveNodeGraphics;
    double moveX, moveY;
};

} // namespace bw.

#endif
