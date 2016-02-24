QMAKE_MAC_SDK = macosx10.9
QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc+ -std=c++11
CONFIG += c++11
QT += widgets
TARGET = BlackAndWhite
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    DefaultMaxWhiteMapViewFactory.cpp \
    MaxWhiteMapTableView.cpp \
    BWTreeColoringController.cpp

HEADERS += \
    MainWindow.h \
    Tree.h \
    TreeView.h \
    TreeItemGraphics.h \
    TreeModel.h \
    DefaultColoringInputDataView.h \
    ColoringInputDataView.h \
    MaxWhiteMapView.h \
    MaxWhiteMapViewFactory.h \
    MaxWhiteMapTableView.h \
    DefaultMaxWhiteMapViewFactory.h \
    TreeViewFactory.h \
    DefaultTreeViewFactory.h \
    ColoringInputDataViewFactory.h \
    DefaultColoringInputDataViewFactory.h \
    PerfomanceComparator.h \
    PerfomanceComparisonView.h \
    Polynomial.h \
    Summator.h \
    MaxWhiteMap.h \
    BWTreeColoring.h \
    BWTreeColorer.h \
    BerendZackerBWTreeColorer.h \
    BWTreeColorerFactory.h \
    BWTreeColoringController.h \
    DefaultBWTreeColorerFactory.h \
    Polynomial_priv.h \
    TreeData.h \
    TreeNode.h \
    StdBWTreeColorer.h \
    AverageBWTreeColorer.h \
    ConjectureBWTreeColorer.h

RESOURCES += \
    Resources.qrc

macx {
    QMAKE_MAC_SDK = macosx10.11
}





















































































































