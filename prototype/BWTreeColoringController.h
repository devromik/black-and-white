#ifndef BW_BW_TREE_COLORING_CONTROLLER_H
#define BW_BW_TREE_COLORING_CONTROLLER_H

#include <vector>
#include <QObject>
#include "TreeNode.h"
#include "ColoringInputDataView.h"
#include "MaxWhiteMapView.h"
#include "TreeView.h"
#include "BWTreeColorer.h"

namespace bw {

class BWTreeColoringController : public QObject {

Q_OBJECT

public:

    typedef TreeModel<TreeNode>::TreeViewIterator TreeViewIterator;

public:

    class MaxWhiteMapViewIterator {

    public:

        MaxWhiteMapViewIterator();

        bool hasNext() const {
            return nextViewIndex < viewCount;
        }

        MaxWhiteMapView* getNext() {
            ++nextViewIndex;
            return views[nextViewIndex - 1];
        }

    private:

        friend class BWTreeColoringController;

    private:

        MaxWhiteMapViewIterator(const std::vector<MaxWhiteMapView*>& views);

    private:

        const std::vector<MaxWhiteMapView*>& views;
        int viewCount;
        int nextViewIndex;
    };

public:

    BWTreeColoringController();

    void setColoringInputDataView(ColoringInputDataView<TreeNode>* newInputDataView) {
        inputDataView = newInputDataView;
    }

    ColoringInputDataView<TreeNode>* getColoringInputDataView() const {
        return inputDataView;
    }

    void addMaxWhiteMapView(MaxWhiteMapView* newMaxWhiteMapView) {
        maxWhiteMapViews.push_back(newMaxWhiteMapView);
    }

    MaxWhiteMapViewIterator maxWhiteMapViewIterator() const {
        return MaxWhiteMapViewIterator(maxWhiteMapViews);
    }

    void addTreeView(TreeView<TreeNode>* newTreeView) {
        treeModel->addView(newTreeView);
    }

    TreeViewIterator treeViewIterator() const {
        return treeModel->treeViewIterator();
    }

    ~BWTreeColoringController();

public slots:

    void color();

private:

    ColoringInputDataView<TreeNode>* inputDataView;
    std::vector<MaxWhiteMapView*> maxWhiteMapViews;
    TreeModel<TreeNode>* treeModel;
    BWTreeColorer<TreeNode>* colorer;
};

} // namespace bw.

#endif
