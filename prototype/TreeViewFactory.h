#ifndef BW_TREE_VIEW_FACTORY_H
#define BW_TREE_VIEW_FACTORY_H

namespace bw {

template<typename NodeType>
class TreeView;

template<typename NodeType>
class TreeViewFactory {

public:

    enum ViewType {
        GRAPHICAL,
        VIEW_TYPE_COUNT
    };

public:

    virtual TreeView<NodeType>* create(const ViewType viewType) const = 0;

    virtual ~TreeViewFactory() {}
};

} // namespace bw.

#endif
