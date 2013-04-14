#include "mainwindow.h"
#include "plotwidget.h"
#include <qtgui>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    auto* v = new QVBoxLayout(this);
    QLabel* label = new QLabel(tr("2010 Chile Earthquake Response Spectrum"));
    label->setAlignment(Qt::AlignCenter);

    setStyleSheet("background-color : white");

    QFont ft = font();
    ft.setPointSizeF(16);
    label->setFont(ft);
    v->addWidget(label);

    PlotWidget* plot = new PlotWidget;
    v->addWidget(plot);
    v->setSpacing(20);

    setWindowTitle("CV6103 - GFM & KA");
}

MainWindow::~MainWindow()
{

}
