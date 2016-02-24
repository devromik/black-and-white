#ifndef BW_DEFAULT_COLORING_INPUT_DATA_VIEW_H
#define BW_DEFAULT_COLORING_INPUT_DATA_VIEW_H

#include <limits>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QIntValidator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ColoringInputDataView.h"
#include "BWTreeColorerFactory.h"

namespace bw {

template<typename NodeType>
class DefaultColoringInputDataView : public ColoringInputDataView<NodeType> {

public:

    DefaultColoringInputDataView(BWTreeColorerFactory<NodeType>* newColorerFactory) : colorerFactory(newColorerFactory) {
        init();
    }

    BWTreeColorer<NodeType>* getColorer() const {
        typedef typename BWTreeColorerFactory<NodeType>::BWTreeColorerType ColorerType;
        return colorerFactory->create((ColorerType)colorerComboBox->itemData(colorerComboBox->currentIndex()).toInt());
    }

    int getBlackCount() const {
        return blackCountEdit->text().toInt();
    }

    int getWhiteCount() const {
        return whiteCountEdit->text().toInt();
    }

    QWidget* getWidget() const {
        return inputDataGroupBox;
    }

private:

    void init() {
        createChildViews();
        arrangeChildViews();
    }

    void createChildViews() {
        blackCountLabel = new QLabel(QObject::tr("чёрных узлов:"));
        blackCountEdit = new QLineEdit();
        blackCountEdit->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), blackCountEdit));

        whiteCountLabel = new QLabel(QObject::tr("белых узлов:"));
        whiteCountEdit = new QLineEdit();
        whiteCountEdit->setValidator(new QIntValidator(0, std::numeric_limits<int>::max(), whiteCountEdit));

        colorerComboBox = new QComboBox();
        colorerComboBox->addItem(QObject::tr("Алгоритм Беренда-Заккера"), BWTreeColorerFactory<NodeType>::BEREND_ZACKER);
        colorerComboBox->addItem(QObject::tr("Алгоритм-гипотеза 1"), BWTreeColorerFactory<NodeType>::AVERAGE);
        colorerComboBox->addItem(QObject::tr("Алгоритм-гипотеза 2"), BWTreeColorerFactory<NodeType>::CONJECTURE);

        inputDataGroupBox = new QGroupBox(QObject::tr("Входные данные"));
    }

    void arrangeChildViews() {
        QHBoxLayout* blackCountLayout = new QHBoxLayout();
        blackCountLayout->addWidget(blackCountLabel);
        blackCountLayout->addSpacing(10);
        blackCountLayout->addWidget(blackCountEdit);

        QHBoxLayout* whiteCountLayout = new QHBoxLayout();
        whiteCountLayout->addWidget(whiteCountLabel);
        whiteCountLayout->addSpacing(10);
        whiteCountLayout->addWidget(whiteCountEdit);

        QVBoxLayout* inputDataLayout = new QVBoxLayout();
        inputDataLayout->addLayout(blackCountLayout);
        inputDataLayout->addLayout(whiteCountLayout);
        inputDataLayout->addSpacing(10);
        inputDataLayout->addWidget(colorerComboBox);

        inputDataGroupBox->setLayout(inputDataLayout);
    }

private:

    // Фабрика чёрно-белых раскрашивателей деревьев.
    BWTreeColorerFactory<NodeType>* colorerFactory;

    // Метка и поле редактирования для числа чёрных узлов в раскраске.
    QLabel* blackCountLabel;
    QLineEdit* blackCountEdit;

    // Метка и поле редактирования для числа белых узлов в раскраске.
    QLabel* whiteCountLabel;
    QLineEdit* whiteCountEdit;

    // Выпадающий список для выбора раскрашивателя.
    QComboBox* colorerComboBox;

    // Группа видов входных данных.
    QGroupBox* inputDataGroupBox;
};

} // namespace bw.

#endif
