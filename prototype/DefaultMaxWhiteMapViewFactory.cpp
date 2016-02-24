#include "DefaultMaxWhiteMapViewFactory.h"
#include "MaxWhiteMapTableView.h"

namespace bw {

DefaultMaxWhiteMapViewFactory* DefaultMaxWhiteMapViewFactory::instance = 0;

MaxWhiteMapView* DefaultMaxWhiteMapViewFactory::create(const ViewType viewType) const {
    if (viewType == TABLE) {
        return new MaxWhiteMapTableView();
    }

    return 0;
}

} // namespace bw.
