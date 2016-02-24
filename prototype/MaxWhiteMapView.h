#ifndef BW_MAX_WHITE_MAP_VIEW_H
#define BW_MAX_WHITE_MAP_VIEW_H

#include <QWidget>
#include "MaxWhiteMap.h"

namespace bw {

class MaxWhiteMapView {

public:

    virtual void setMap(MaxWhiteMap* newMap) = 0;

    virtual QWidget* getWidget() = 0;

    virtual ~MaxWhiteMapView() {}
};

} // namespace bw.

#endif
