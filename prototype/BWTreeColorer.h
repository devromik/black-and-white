#ifndef BW_BW_TREE_COLORER_H
#define BW_BW_TREE_COLORER_H

#include "Tree.h"
#include "MaxWhiteMap.h"
#include "BWTreeColoring.h"

namespace bw {

template<typename NodeType>
class BWTreeColorer {

public:

    typedef Tree<NodeType> TreeType;
    typedef BWTreeColoring<NodeType> ColoringType;

public:

    virtual void setTree(TreeType* tree) = 0;

    virtual bool coloringExists(const int blackCount, const int whiteCount) = 0;
    virtual MaxWhiteMap* getMaxWhiteMap() = 0;
    virtual ColoringType* getColoring(const int blackCount, const int whiteCount) = 0;

    virtual ~BWTreeColorer() {}
};

} // namespace bw.

#endif
