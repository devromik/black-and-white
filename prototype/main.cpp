#include <QTextCodec>
#include <QApplication>
#include "DefaultTreeViewFactory.h"
#include "DefaultMaxWhiteMapViewFactory.h"
#include "DefaultColoringInputDataViewFactory.h"
#include "DefaultBWTreeColorerFactory.h"
#include "BWTreeColoringController.h"
#include "MainWindow.h"

void init() {
    QTextCodec* textCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(textCodec);
}

int main(int argCount, char* args[]) {    
    QApplication blackAndWhiteApp(argCount, args);
    init();

    bw::BWTreeColoringController* controller = new bw::BWTreeColoringController();
    controller->setColoringInputDataView(bw::DefaultColoringInputDataViewFactory<bw::TreeNode>::getInstance()->create(bw::ColoringInputDataViewFactory<bw::TreeNode>::DEFAULT, bw::DefaultBWTreeColorerFactory<bw::TreeNode>::getInstance()));
    controller->addMaxWhiteMapView(bw::DefaultMaxWhiteMapViewFactory::getInstance()->create(bw::MaxWhiteMapViewFactory::TABLE));
    controller->addTreeView(bw::DefaultTreeViewFactory<bw::TreeNode>::getInstance()->create(bw::TreeViewFactory<bw::TreeNode>::GRAPHICAL));

    bw::MainWindow mainWin(0, controller);
    mainWin.show();

    return blackAndWhiteApp.exec();
}
