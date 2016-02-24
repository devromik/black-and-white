#ifndef BW_DEFAULT_COLORING_INPUT_DATA_VIEW_FACTORY_H
#define BW_DEFAULT_COLORING_INPUT_DATA_VIEW_FACTORY_H

#include "ColoringInputDataViewFactory.h"
#include "DefaultColoringInputDataView.h"

namespace bw {

template<typename NodeType>
class DefaultColoringInputDataViewFactory : public ColoringInputDataViewFactory<NodeType> {

public:

    typedef ColoringInputDataViewFactory<NodeType> ColoringInputDataViewFactoryType;
    typedef typename ColoringInputDataViewFactoryType::ViewType ViewType;

public:

    static DefaultColoringInputDataViewFactory* getInstance() {
        if (instance == 0) {
            instance = new DefaultColoringInputDataViewFactory();
        }

        return instance;
    }

    ColoringInputDataView<NodeType>* create(const ViewType viewType, BWTreeColorerFactory<NodeType>* colorerFactory) const {
        if (viewType == ColoringInputDataViewFactoryType::DEFAULT) {
            return new DefaultColoringInputDataView<NodeType>(colorerFactory);
        }

        return 0;
    }

protected:

    DefaultColoringInputDataViewFactory() {}
    DefaultColoringInputDataViewFactory(const DefaultColoringInputDataViewFactory& source) {}
    DefaultColoringInputDataViewFactory<NodeType>& operator = (const DefaultColoringInputDataViewFactory& source) {}

    virtual ~DefaultColoringInputDataViewFactory() {}

private:

    static DefaultColoringInputDataViewFactory* instance;
};

template<typename NodeType>
DefaultColoringInputDataViewFactory<NodeType>* DefaultColoringInputDataViewFactory<NodeType>::instance = 0;

} // namespace bw.

#endif
