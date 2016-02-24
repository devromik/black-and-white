#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "MainWindow.h"

namespace bw {

MainWindow::MainWindow(
    QWidget* parent,
    BWTreeColoringController* newController) :

    QWidget(parent),
    controller(newController) {

    init();
}

MainWindow::~MainWindow() {
    delete controller;
}

void MainWindow::init() {
    createChildViews();
    arrangeChildViews();

    setWindowIcon(QIcon(":/Pictures/Icons/AppIcon.png"));
    setWindowState(windowState() | Qt::WindowMaximized);
}

void MainWindow::createChildViews() {
    startColoringButton = new QPushButton(tr("Раскрасить"));
    connect(startColoringButton, SIGNAL(clicked()), controller, SLOT(color()));
}

void MainWindow::arrangeChildViews() {
    QVBoxLayout* inputDataLayout = new QVBoxLayout();
    inputDataLayout->addWidget(controller->getColoringInputDataView()->getWidget(), 5);
    inputDataLayout->addSpacing(20);
    inputDataLayout->addWidget(startColoringButton, 2);

    QVBoxLayout* maxWhiteMapLayout = new QVBoxLayout();
    maxWhiteMapLayout->addSpacing(20);
    BWTreeColoringController::MaxWhiteMapViewIterator maxWhiteMapViewIter = controller->maxWhiteMapViewIterator();

    while (maxWhiteMapViewIter.hasNext()) {
        maxWhiteMapLayout->addWidget(maxWhiteMapViewIter.getNext()->getWidget());
    }

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addLayout(inputDataLayout, 2);
    topLayout->addSpacing(10);
    topLayout->addLayout(maxWhiteMapLayout, 8);
    topLayout->setAlignment(maxWhiteMapLayout, Qt::AlignTop);

    QHBoxLayout* treeLayout = new QHBoxLayout();
    BWTreeColoringController::TreeViewIterator treeViewIter = controller->treeViewIterator();

    while (treeViewIter.hasNext()) {
        treeLayout->addWidget(treeViewIter.getNext());
    }

    QVBoxLayout* topLevelLayout = new QVBoxLayout();
    topLevelLayout->addLayout(topLayout, 2);
    topLevelLayout->addSpacing(10);
    topLevelLayout->addLayout(treeLayout, 8);

    setLayout(topLevelLayout);
}

} // namespace bw.



