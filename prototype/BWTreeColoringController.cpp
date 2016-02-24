#include "BWTreeColoringController.h"

#include <QMessageBox>
#include "BWTreeColoring.h"

namespace bw {

/* ********** BWTreeColoringController::MaxWhiteMapViewIterator ********** */

BWTreeColoringController::MaxWhiteMapViewIterator::MaxWhiteMapViewIterator(const std::vector<MaxWhiteMapView*>& newViews) :
    views(newViews),
    viewCount(newViews.size()),
    nextViewIndex(0) {}

/* ********** BWTreeColoringController ********** */

BWTreeColoringController::BWTreeColoringController() :
    inputDataView(0),
    treeModel(new TreeModel<TreeNode>()),
    colorer(0) {}

void BWTreeColoringController::color() {
    colorer = inputDataView->getColorer();
    colorer->setTree(treeModel->getTree());

    auto maxWhiteMapViewIter = maxWhiteMapViewIterator();

    while (maxWhiteMapViewIter.hasNext()) {
        maxWhiteMapViewIter.getNext()->setMap(colorer->getMaxWhiteMap());
    }

    int blackCount = inputDataView->getBlackCount();
    int whiteCount = inputDataView->getWhiteCount();

    if (colorer->coloringExists(blackCount, whiteCount)) {
        treeModel->setColoring(colorer->getColoring(blackCount, whiteCount));
    }
    else {
        QMessageBox::warning(
            0,
            "",
            QObject::tr("Невозможно раскрасить ") +
            QString::number(blackCount) +
            QObject::tr(" узлов в чёрный цвет и ") +
            QString::number(whiteCount) +
            QObject::tr(" узлов - в белый. Смотрите таблицу."),
            QMessageBox::Ok);
    }
}

BWTreeColoringController::~BWTreeColoringController() {
    delete inputDataView;
    auto maxWhiteMapViewIter = maxWhiteMapViewIterator();

    while (maxWhiteMapViewIter.hasNext()) {
        delete maxWhiteMapViewIter.getNext();
    }

    delete treeModel;
    delete colorer;
}

} // namespace bw.






















