#ifndef BW_MAX_WHITE_MAP_VIEW_FACTORY_H
#define BW_MAX_WHITE_MAP_VIEW_FACTORY_H

namespace bw {

class MaxWhiteMapView;

class MaxWhiteMapViewFactory {

public:

    enum ViewType {
        TABLE,
        VIEW_TYPE_COUNT
    };

public:

    virtual MaxWhiteMapView* create(const ViewType viewType) const = 0;

    virtual ~MaxWhiteMapViewFactory() {}
};

} // namespace bw.

#endif
