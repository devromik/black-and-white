#ifndef BW_TREE_DATA_H
#define BW_TREE_DATA_H

#include <memory>
#include <map>

namespace bw {

/**
 * Данные дерева (сопутствующие данные узлов дерева).
 */
template<typename NodeType, typename DataType, typename MapType = std::map<NodeType*, DataType> >
class TreeData {

public:

    TreeData() : nodeToData(std::make_shared<MapType>()) {}

    void set(NodeType* node, const DataType& data) {
        if (has(node)) {
            remove(node);
        }

        (*nodeToData)[node] = data;
    }

    const DataType& get(NodeType* node) const {
        return (*nodeToData)[node];
    }

    bool has(NodeType* node) const {
        return nodeToData->find(node) != nodeToData->end();
    }

    void remove(NodeType* node) {
        if (has(node)) {
            nodeToData->erase(nodeToData->find(node));
        }
    }

    void clear() {
        nodeToData->clear();
    }

private:

    // Отображение узлов на сопутствующие им данные.
    std::shared_ptr<MapType> nodeToData;
};

} // namespace bw.

#endif

















