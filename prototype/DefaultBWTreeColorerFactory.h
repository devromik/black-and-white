#ifndef BW_DEFAULT_BW_TREE_COLORER_FACTORY_H
#define BW_DEFAULT_BW_TREE_COLORER_FACTORY_H

#include "BWTreeColorerFactory.h"
#include "BerendZackerBWTreeColorer.h"
#include "AverageBWTreeColorer.h"
#include "ConjectureBWTreeColorer.h"

namespace bw {

template<typename NodeType>
class DefaultBWTreeColorerFactory : public BWTreeColorerFactory<NodeType> {

public:

    typedef BWTreeColorerFactory<NodeType> BWTreeColorerFactoryType;
    typedef typename BWTreeColorerFactoryType::BWTreeColorerType BWTreeColorerType;

public:

    static DefaultBWTreeColorerFactory* getInstance() {
        if (instance == 0) {
            instance = new DefaultBWTreeColorerFactory();
        }

        return instance;
    }

    BWTreeColorer<NodeType>* create(const BWTreeColorerType colorerType) const {
        if (colorerType == BWTreeColorerFactoryType::BEREND_ZACKER) {
            return new BerendZackerBWTreeColorer<NodeType>();
        }
        else if (colorerType == BWTreeColorerFactoryType::AVERAGE) {
            return new AverageBWTreeColorer<NodeType>();
        }
        else if (colorerType == BWTreeColorerFactoryType::CONJECTURE) {
            return new ConjectureBWTreeColorer<NodeType>();
        }

        return 0;
    }

protected:

    DefaultBWTreeColorerFactory() {}
    DefaultBWTreeColorerFactory(const DefaultBWTreeColorerFactory& source) {}
    DefaultBWTreeColorerFactory& operator = (const DefaultBWTreeColorerFactory& source) {}

private:

    static DefaultBWTreeColorerFactory* instance;
};

template<typename NodeType>
DefaultBWTreeColorerFactory<NodeType>* DefaultBWTreeColorerFactory<NodeType>::instance = 0;

} // namespace bw.

#endif
