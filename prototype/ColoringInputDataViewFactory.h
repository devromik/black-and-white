#ifndef BW_COLORING_INPUT_DATA_VIEW_FACTORY_H
#define BW_COLORING_INPUT_DATA_VIEW_FACTORY_H

namespace bw {

template<typename NodeType>
class ColoringInputDataView;

template<typename NodeType>
class BWTreeColorerFactory;

template<typename NodeType>
class ColoringInputDataViewFactory {

public:

    enum ViewType {
        DEFAULT,
        VIEW_TYPE_COUNT
    };

public:

    virtual ColoringInputDataView<NodeType>* create(const ViewType viewType, BWTreeColorerFactory<NodeType>* colorerFactory) const = 0;

    virtual ~ColoringInputDataViewFactory() {}
};

} // namespace bw.

#endif
