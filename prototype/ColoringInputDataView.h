#ifndef BW_COLORING_INPUT_DATA_VIEW_H
#define BW_COLORING_INPUT_DATA_VIEW_H

#include <QWidget>
#include "BWTreeColorer.h"

namespace bw {

template<typename NodeType>
class ColoringInputDataView {

public:

    virtual BWTreeColorer<NodeType>* getColorer() const = 0;

    virtual int getBlackCount() const = 0;
    virtual int getWhiteCount() const = 0;

    virtual QWidget* getWidget() const = 0;
};

} // namespace bw.

#endif
