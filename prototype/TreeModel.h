#ifndef BW_TREE_MODEL_H
#define BW_TREE_MODEL_H

#include <vector>
#include <map>
#include <QString>
#include <QtGlobal>
#include <QtDebug>
#include "Tree.h"
#include "TreeItemGraphics.h"
#include "BWTreeColoring.h"

namespace bw {

template<typename NodeType>
class TreeView;

/**
 * Модель дерева.
 */
template<typename NodeType>
class TreeModel {

public:

    typedef Tree<NodeType> TreeType;
    typedef typename TreeType::IsLeafPredicate IsLeafPredicate;
    typedef NodeGraphics<NodeType> NodeGraphicsType;
    typedef EdgeGraphics<NodeType> EdgeGraphicsType;
    typedef TreeView<NodeType> TreeViewType;
    typedef BWTreeColoring<NodeType> ColoringType;
    typedef typename ColoringType::Color ColorType;

public:

    /**
     * Итератор по видам дерева.
     */
    class TreeViewIterator {

    public:

        bool hasNext() const {
            return nextViewIndex < viewCount;
        }

        TreeView<NodeType>* getNext() {
            ++nextViewIndex;
            return views[nextViewIndex - 1];
        }

    private:

        friend class TreeModel;

    private:

        TreeViewIterator(const std::vector<TreeViewType*>& newViews) :
            views(newViews),
            viewCount(newViews.size()),
            nextViewIndex(0) {}

    private:

        const std::vector<TreeViewType*>& views;
        int viewCount;
        int nextViewIndex;
    };

public:

    TreeModel() : tree(0), coloring(0), scene(new QGraphicsScene()) {
        createTree();
    }

    void addView(TreeViewType* newTreeView) {
        newTreeView->setTreeModel(this);
        views.push_back(newTreeView);
    }

    NodeGraphicsType* createChildNode(NodeGraphicsType* parentNodeGraphics, const double childSceneX, const double childSceneY) {
        NodeType* newParent = parentNodeGraphics->getNode();
        NodeType* newChild = tree->add(newParent);

        NodeGraphicsType* newChildGraphics = new NodeGraphicsType(newChild);
        nodeGraphics[newChild] = newChildGraphics;
        EdgeGraphicsType* newEdgeGraphics = new EdgeGraphicsType(parentNodeGraphics, newChildGraphics);
        parentNodeGraphics->addEdgeGraphics(newEdgeGraphics);
        newChildGraphics->addEdgeGraphics(newEdgeGraphics);

        scene->addItem(newChildGraphics);
        scene->addItem(newEdgeGraphics);

        newChildGraphics->setPos(childSceneX, childSceneY);
        newEdgeGraphics->nodesUpdated();

        return newChildGraphics;
    }

    NodeGraphicsType* createChildNode(NodeGraphicsType* parentNodeGraphics, const QPointF& childScenePos) {
        return createChildNode(parentNodeGraphics, childScenePos.x(), childScenePos.y());
    }

    void removeSubtree(NodeGraphicsType* rootGraphics) {
        typedef typename TreeType::template PostorderIterator<IsLeafPredicate> PostorderIterator;
        PostorderIterator treeIter = tree->template postorderIterator<IsLeafPredicate>(rootGraphics->getNode());

        while (treeIter.hasNext()) {
            NodeType* node = treeIter.getNext();
            NodeGraphicsType* graphics = nodeGraphics[node];

            if (node->hasParent()) {
                EdgeGraphicsType* edgeGraphics = graphics->getEdgeGraphics(0);
                nodeGraphics[node->getParent()]->removeEdgeGraphics(edgeGraphics);
                scene->removeItem(graphics);
                scene->removeItem(edgeGraphics);
                nodeGraphics.erase(node);
            }
        }

        tree->remove(rootGraphics->getNode());
        scene->update();
    }

    void moveSubtree(NodeGraphicsType* rootGraphics, const double dx, const double dy) {
        typedef typename TreeType::template Iterator<IsLeafPredicate> Iterator;
        Iterator treeIter = tree->template iterator<IsLeafPredicate>(rootGraphics->getNode());

        while (treeIter.hasNext()) {
            NodeType* node = treeIter.getNext();
            NodeGraphicsType* graphics = nodeGraphics[node];
            graphics->move(dx, dy);
        }

        scene->update();
    }

    TreeType* getTree() const {
        return tree;
    }

    void setColoring(ColoringType* newColoring) {
        if (coloring != 0) {
            delete coloring;
        }

        coloring = newColoring;
        NodeGraphicsIterator nodeGraphicsIter(scene->items());

        while (nodeGraphicsIter.hasNext()) {
            NodeGraphicsType* nodeGraphics = nodeGraphicsIter.getNext();
            ColorType color = coloring->getColor(nodeGraphics->getNode());

            if (color == ColoringType::BLACK) {
                nodeGraphics->setBackgroundBrush(Qt::black);
            }
            else if (color == ColoringType::WHITE) {
                nodeGraphics->setBackgroundBrush(Qt::white);
            }
            else {
                nodeGraphics->setBackgroundBrush(QColor(217, 217, 217));
            }
        }

        scene->update();
    }

    QGraphicsScene* getScene() const {
        return scene;
    }

    TreeViewIterator treeViewIterator() const {
        return TreeViewIterator(views);
    }

    ~TreeModel() {
        delete tree;
        delete scene;
        TreeViewIterator treeViewIter = treeViewIterator();

        while (treeViewIter.hasNext()) {
            delete treeViewIter.getNext();
        }
    }

private:

    /**
     * Итератор по графическим представлениям узлов.
     */
    class NodeGraphicsIterator {

    public:

        NodeGraphicsIterator(const QList<QGraphicsItem*>& newGraphics) :
            graphics(newGraphics),
            graphicsCount(newGraphics.count()),
            nextNodeGraphics(-1) {

            findNextNodeGraphics();
        }

        bool hasNext() const {
            return nextNodeGraphics < graphicsCount;
        }

        NodeGraphicsType* getNext() {
            NodeGraphicsType* currNodeGraphics = (NodeGraphicsType*)graphics[nextNodeGraphics];
            findNextNodeGraphics();

            return currNodeGraphics;
        }

    private:

        void findNextNodeGraphics() {
            ++nextNodeGraphics;

            while (nextNodeGraphics < graphicsCount && dynamic_cast<NodeGraphicsType*>(graphics[nextNodeGraphics]) == 0) {
                ++nextNodeGraphics;
            }
        }

    private:

        // Графические элементы сцены.
        QList<QGraphicsItem*> graphics;

        // Количество графических элементов в сцене.
        int graphicsCount;

        // Индекс текущего графического представления узла.
        int nextNodeGraphics;
    };

private:

    void createTree() {
        if (coloring != 0) {
            delete coloring;
        }

        scene->clear();
        scene->update();

        if (tree != 0) {
            delete tree;
        }

        NodeType* root = new NodeType();
        tree = new TreeType(root);

        NodeGraphicsType* rootGraphics = new NodeGraphicsType(root);
        scene->addItem(rootGraphics);

        nodeGraphics[root] = rootGraphics;
    }

private:

    // Дерево.
    TreeType* tree;

    // Чёрно-белая раскраска дерева.
    ColoringType* coloring;

    // Сцена.
    QGraphicsScene* scene;

    // Виды дерева.
    std::vector<TreeViewType*> views;

    // Отображение узлов на их представления.
    std::map<NodeType*, NodeGraphicsType*> nodeGraphics;
};

} // namespace bw.

#endif
