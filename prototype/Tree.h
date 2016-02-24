#ifndef BW_TREE_H
#define BW_TREE_H

#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <QString>
#include "TreeData.h"

namespace bw {

/**
 * Дерево.
 */
template<typename NodeType>
class Tree {

public:

    typedef Tree<NodeType> ThisType;

public:

    /**
     * Предикат узлов дерева,
     * срабатывающий в том и только в том случае,
     * когда узел-аргумент является листом дерева.
     */
    class IsLeafPredicate : public std::unary_function<NodeType*, bool> {

    public:

        bool operator()(NodeType* node) const {
            return node->isLeaf();
        }
    };

    /**
     * Итератор по дереву в прямом порядке (узел-предок обходится прежде своих потомков).
     */
    template<typename IsLeafPredicateType = ThisType::IsLeafPredicate>
    class PreorderIterator {

    public:

        PreorderIterator(NodeType* root) {
            if (root != 0) {
                traversalOrder.push(root);
            }
        }

        PreorderIterator(const PreorderIterator& source) : traversalOrder(source.traversalOrder) {}

        bool hasNext() const {
            return !traversalOrder.empty();
        }

        NodeType* getNext() {
            NodeType* next = traversalOrder.front();
            traversalOrder.pop();

            if (!isLeafPredicate(next)) {
                for (int i = 0; i < next->getChildCount(); ++i) {
                    traversalOrder.push((NodeType*)next->getChild(i));
                }
            }

            return next;
        }

    private:

        friend class Tree;

    private:

        std::queue<NodeType*> traversalOrder;
        IsLeafPredicateType isLeafPredicate;
    };

    /**
     * Итератор по дереву в обратном порядке (узел-предок обходится после своих потомков).
     */
    template<typename IsLeafPredicateType = ThisType::IsLeafPredicate>
    class PostorderIterator {

    public:

        PostorderIterator(NodeType* root) {
            if (root != 0) {
                traversalOrder.push(root);
            }
        }

        PostorderIterator(const PostorderIterator& source) : traversalOrder(source.traversalOrder) {}

        bool hasNext() const {
            return !traversalOrder.empty();
        }

        NodeType* getNext() {
            while (true) {
                NodeType* top = traversalOrder.top();

                for (int i = 0; i < top->getChildCount(); ++i) {
                    if (traversedNodes.find(top->getChild(i)) == traversedNodes.end()) {
                        traversalOrder.push(top->getChild(i));
                        break;
                    }
                }

                if (traversalOrder.top() == top) {
                    traversedNodes.insert(top);
                    traversalOrder.pop();

                    return top;
                }
            }
        }

    private:

        friend class Tree;

    private:

        std::stack<NodeType*> traversalOrder;
        std::set<NodeType*> traversedNodes;
        IsLeafPredicateType isLeafPredicate;
    };

    template<typename IsLeafPredicateType = ThisType::IsLeafPredicate>
    class Iterator : public PreorderIterator<IsLeafPredicateType> {

    public:

        typedef PreorderIterator<IsLeafPredicateType> BaseType;

    public:

        Iterator(NodeType* root) : BaseType(root) {}
        Iterator(const Iterator& source) : BaseType(source) {}

    private:

        friend class Tree;
    };

public:

    Tree() : root(0), nodeCount(0) {}

    Tree(NodeType* newRoot) : root(0) {
        setRoot(newRoot);
    }

    virtual ~Tree() {
        clear();
    }

    bool setRoot(NodeType* newRoot) {
        if (newRoot == 0) {
            clear();
            root = newRoot;
            nodeCount = 0;

            return true;
        }
        else if (newRoot->isLeaf()) {
            clear();
            root = newRoot;
            root->setParent(0);
            nodeCount = 1;

            return true;
        }
        else {
            return false;
        }
    }

    NodeType* getRoot() const {
        return root;
    }

    bool add(NodeType* newNode, NodeType* parent) {
        if (newNode->isLeaf()) {
            if (parent != 0) {
                parent->addChild(newNode);
                ++nodeCount;
            }
            else {
                setRoot(newNode);
            }

            return true;
        }
        else {
            return false;
        }
    }

    NodeType* add(NodeType* parent) {
        NodeType* newNode = new NodeType();

        if (parent != 0) {
            parent->addChild(newNode);
            ++nodeCount;
        }
        else {
            setRoot(newNode);
        }

        return newNode;
    }

    int getNodeCount() const {
        return nodeCount;
    }

    int getSubtreeSize(NodeType* subtreeRoot) const {
        if (subtreeRoot == 0) {
            return 0;
        }

        int subtreeSize = 0;
        Iterator<IsLeafPredicate> subtreeIter = iterator<IsLeafPredicate>(subtreeRoot);

        while (subtreeIter.hasNext()) {
            subtreeIter.getNext();
            ++subtreeSize;
        }

        return subtreeSize;
    }

    bool isEmpty() const {
        return getNodeCount() == 0;
    }

    template<typename IsLeafPredicateType>
    PreorderIterator<IsLeafPredicateType> preorderIterator() const {
        return PreorderIterator<IsLeafPredicateType>(root);
    }

    template<typename IsLeafPredicateType>
    PreorderIterator<IsLeafPredicateType> preorderIterator(NodeType* subtreeRoot) const {
        return PreorderIterator<IsLeafPredicateType>(subtreeRoot);
    }

    template<typename IsLeafPredicateType>
    PostorderIterator<IsLeafPredicateType> postorderIterator() const {
        return PostorderIterator<IsLeafPredicateType>(root);
    }

    template<typename IsLeafPredicateType>
    PostorderIterator<IsLeafPredicateType> postorderIterator(NodeType* subtreeRoot) const {
        return PostorderIterator<IsLeafPredicateType>(subtreeRoot);
    }

    template<typename IsLeafPredicateType>
    Iterator<IsLeafPredicateType> iterator() const {
        return Iterator<IsLeafPredicateType>(root);
    }

    template<typename IsLeafPredicateType>
    Iterator<IsLeafPredicateType> iterator(NodeType* subtreeRoot) const {
        return Iterator<IsLeafPredicateType>(subtreeRoot);
    }

    void remove(NodeType* node) {
        if (node == 0) {
            return;
        }

        if (node->hasParent()) {
            node->getParent()->removeChild(node);
        }

        nodeCount -= getSubtreeSize(node);
        delete node;
    }

    void clear() {
        if (root != 0) {
            delete root;
            root = 0;
            nodeCount = 0;
        }
    }

    /**
     * Возвращает самый левый потомок данного узла.
     */
    NodeType* getLeftmostAncestor(NodeType* node) const {
        if (node == 0) {
            return 0;
        }

        while (!node->isLeaf()) {
            node = node->getLeftmostChild();
        }

        return node;
    }

    /**
     * Возвращает самый правый потомок данного узла.
     */
    NodeType* getRightmostAncestor(NodeType* node) const {
        if (node == 0) {
            return 0;
        }

        while (!node->isLeaf()) {
            node = node->getRightmostChild();
        }

        return node;
    }

    /**
     * Возвращает отображение узлов на их самые левые потомки.
     */
    TreeData<NodeType, NodeType*> getLeftmostAncestors() const {
        TreeData<NodeType, NodeType*> leftmostAncestors;

        if (isEmpty()) {
            return leftmostAncestors;
        }

        PostorderIterator<IsLeafPredicate> postorderIter = postorderIterator<IsLeafPredicate>();

        while (postorderIter.hasNext()) {
            NodeType* node = postorderIter.getNext();
            leftmostAncestors.set(node, node->isLeaf() ? node : leftmostAncestors.get(node->getLeftmostChild()));
        }

        return leftmostAncestors;
    }

    /**
     * Возвращает отображение узлов на их самые правые потомки.
     */
    TreeData<NodeType, NodeType*> getRightmostAncestors() const {
        TreeData<NodeType, NodeType*> rightmostAncestors;

        if (isEmpty()) {
            return rightmostAncestors;
        }

        PostorderIterator<IsLeafPredicate> postorderIter = postorderIterator<IsLeafPredicate>();

        while (postorderIter.hasNext()) {
            NodeType* node = postorderIter.getNext();
            rightmostAncestors.set(node, node->isLeaf() ? node : rightmostAncestors.get(node->getRightmostChild()));
        }

        return rightmostAncestors;
    }

private:

    // Корень.
    NodeType* root;

    // Количество узлов.
    int nodeCount;
};

} // namespace bw.

#endif
