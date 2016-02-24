#ifndef BW_DEFAULT_MAX_WHITE_MAP_VIEW_FACTORY_H
#define BW_DEFAULT_MAX_WHITE_MAP_VIEW_FACTORY_H

#include "MaxWhiteMapViewFactory.h"

namespace bw {

class DefaultMaxWhiteMapViewFactory : public MaxWhiteMapViewFactory {

public:

    static DefaultMaxWhiteMapViewFactory* getInstance() {
        if (instance == 0) {
            instance = new DefaultMaxWhiteMapViewFactory();
        }

        return instance;
    }

    MaxWhiteMapView* create(const ViewType viewType) const;

protected:

    DefaultMaxWhiteMapViewFactory() {}
    DefaultMaxWhiteMapViewFactory(const DefaultMaxWhiteMapViewFactory& source) {}
    DefaultMaxWhiteMapViewFactory& operator = (const DefaultMaxWhiteMapViewFactory& source) {}

    virtual ~DefaultMaxWhiteMapViewFactory() {}

private:

    static DefaultMaxWhiteMapViewFactory* instance;
};

} // namespace bw.

#endif
