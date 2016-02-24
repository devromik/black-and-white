#ifndef BW_BW_TREE_COLORER_FACTORY_H
#define BW_BW_TREE_COLORER_FACTORY_H

#include "BWTreeColorer.h"

namespace bw {

template<typename NodeType>
class BWTreeColorerFactory {

public:

    enum BWTreeColorerType {
        BEREND_ZACKER,
        AVERAGE,
        CONJECTURE,
        COLORER_TYPE_COUNT
    };

public:

    virtual BWTreeColorer<NodeType>* create(const BWTreeColorerType) const = 0;

    virtual ~BWTreeColorerFactory() {}
};

} // namespace bw.

#endif
