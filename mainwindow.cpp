#include "mainwindow.h"
#include "plotwidget.h"
#include <qtgui>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    auto* v = new QVBoxLayout(this);

    QSettings setting("setting.ini", QSettings::IniFormat);
    setting.beginGroup("title");
    QString title = setting.value("title","2010 Chile Earthquake Response Spectrum").toString();
    setting.endGroup();

    QLabel* label = new QLabel(title);
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

    setting.beginGroup("title");
    setting.setValue("title", title);
    setting.endGroup();
}

MainWindow::~MainWindow()
{

}
