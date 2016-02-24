#ifndef BW_DEFAULT_TREE_VIEW_FACTORY_H
#define BW_DEFAULT_TREE_VIEW_FACTORY_H

#include "TreeView.h"
#include "TreeViewFactory.h"

namespace bw {

template<typename NodeType>
class DefaultTreeViewFactory : public TreeViewFactory<NodeType> {

public:

    typedef TreeViewFactory<NodeType> TreeViewFactoryType;
    typedef typename TreeViewFactoryType::ViewType ViewType;

public:

    static DefaultTreeViewFactory* getInstance() {
        if (instance == 0) {
            instance = new DefaultTreeViewFactory();
        }

        return instance;
    }

    TreeView<NodeType>* create(const ViewType viewType) const {
        if (viewType == TreeViewFactoryType::GRAPHICAL) {
            return new TreeView<NodeType>();
        }

        return 0;
    }

protected:

    DefaultTreeViewFactory() {}
    DefaultTreeViewFactory(const DefaultTreeViewFactory& source) {}
    DefaultTreeViewFactory<NodeType>& operator = (const DefaultTreeViewFactory& source) {}

    virtual ~DefaultTreeViewFactory() {}

private:

    static DefaultTreeViewFactory* instance;
};

template<typename NodeType>
DefaultTreeViewFactory<NodeType>* DefaultTreeViewFactory<NodeType>::instance = 0;

} // namespace bw.

#endif
