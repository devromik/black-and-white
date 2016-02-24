#ifndef BW_TREE_NODE_H
#define BW_TREE_NODE_H

#include <vector>
#include <algorithm>
#include <iterator>
#include <QDebug>

namespace bw {

/**
 * Узел дерева.
 */
class TreeNode {

public:

    TreeNode(TreeNode* newParent = 0) : parent(newParent) {}
    TreeNode(const std::vector<TreeNode*>& newChildren) : parent(0), children(newChildren) {}
    TreeNode(TreeNode* newParent, const std::vector<TreeNode*>& newChildren) : parent(newParent), children(newChildren) {}

    bool hasParent() const {
        return parent != 0;
    }

    void setParent(TreeNode* newParent) {
        parent = newParent;
    }

    TreeNode* getParent() const {
        return parent;
    }

    bool isRoot() const {
        return !hasParent();
    }

    bool isLeaf() const {
        return children.empty();
    }

    void addChild(TreeNode* newChild) {
        children.push_back(newChild);
        newChild->setParent(this);
    }

    void addChild(TreeNode* newChild, const int index) {
        if (children.empty()) {
            children.push_back(newChild);
        }
        else {
            children.insert(children.begin() + index, newChild);
        }

        newChild->setParent(this);
    }

    void setChild(TreeNode* newChild, const int index) {
        if (children.empty()) {
            children.push_back(newChild);
        }
        else {
            children[index] = newChild;
        }

        newChild->setParent(this);
    }

    TreeNode* getChild(const int index) const {
        return children[index];
    }

    int getChildPosition(TreeNode* child) const {
        return std::distance(children.begin(), std::find(children.begin(), children.end(), child));
    }

    int getChildCount() const {
        return children.size();
    }

    TreeNode* getLeftmostChild() const {
        return !isLeaf() ? getChild(0) : 0;
    }

    TreeNode* getRightmostChild() const {
        return !isLeaf() ? getChild(getChildCount() - 1) : 0;
    }

    bool hasSingleChild() const {
        return getChildCount() == 1;
    }

    void removeChild(TreeNode* child) {
        child->setParent(0);
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    void removeAllChildren() {
        foreach (auto child, children) {
            child->setParent(0);
        }

        children.clear();
    }

    void deleteAllChildren() {
        foreach (auto child, children) {
            delete child;
        }

        removeAllChildren();
    }

    void printChildren() {
        qDebug() << "***** Node " << QString::number((long long)this) << " children *****";

        for (int i = 0; i < getChildCount(); ++i) {
            qDebug() << "    " << QString::number((long long)getChild(i));
        }

        qDebug() << "******************************";
    }

    ~TreeNode() {
        deleteAllChildren();
    }

private:

    TreeNode(const TreeNode& source) {}
    TreeNode& operator = (const TreeNode& source) {}

private:

    // Родительский узел.
    TreeNode* parent;

    // Дочерние узлы.
    std::vector<TreeNode*> children;
};

} // namespace bw.

#endif
