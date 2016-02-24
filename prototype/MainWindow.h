#ifndef BW_MAIN_WINDOW_H
#define BW_MAIN_WINDOW_H

#include <QWidget>
#include <QPushButton>
#include "BWTreeColoringController.h"

namespace bw {

class MainWindow : public QWidget {

Q_OBJECT

public:

    MainWindow(QWidget* parent, BWTreeColoringController* controller);

    ~MainWindow();

private:

    void init();

    void createChildViews();
    void arrangeChildViews();

private:

    BWTreeColoringController* controller;
    QPushButton* startColoringButton;
};

} // namespace bw.

#endif
