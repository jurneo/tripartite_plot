#include "mainwindow.h"
#include "qwt_plot.h"
#include <qtgui>
#include <qwt_scale_engine.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_map.h>
#include "tripartitegrid.h"
#include <qtimer>
#include <cmath>
#include <iostream>
#include <qwt_plot_canvas.h>
#include "reader.h"
#include <qtconcurrentrun>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>

namespace {
TripartiteGrid* gGrid = 0;
QwtPlot* gPlot = 0;
QColor gColors[] = { Qt::blue, Qt::red, Qt::green, Qt::black };
}

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    QtConcurrent::run([=](){cvgk::read();});

    auto* plot = new QwtPlot(this);
    gPlot = plot;
    QFont txtFont("Helvetica", 10);
    for (int i = 0; i < QwtPlot::axisCnt; i++)
    {
        QwtScaleWidget* scaleWidget = (QwtScaleWidget*) plot->axisWidget(i);
        if (scaleWidget)
        {
            scaleWidget->setMargin(0);
            const int fmh = QFontMetrics(scaleWidget->font()).height();
            scaleWidget->setMinBorderDist(0, fmh);

            // set the fonts
            QwtText text = scaleWidget->title();
            text.setFont(txtFont);
            scaleWidget->setTitle(text);
        }
    }

    auto* v = new QVBoxLayout(this);
    QLabel* label = new QLabel(tr("2010 Chile Earthquake Response Spectrum"));
    label->setAlignment(Qt::AlignCenter);
    QFont ft = font();
    ft.setPointSizeF(16);
    label->setFont(ft);
    v->addWidget(label);
    v->addWidget(plot);
    v->setSpacing(30);
    setWindowTitle("CV6103 - GFM & KA");

    auto xm1 = 0.03;
    auto xm2 = 20;
    auto ym1 = 0.03;
    auto ym2 = 5;

    plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);

    plot->setAxisMaxMajor(QwtPlot::xBottom, 6);
    plot->setAxisMaxMinor(QwtPlot::xBottom, 10);
    plot->setAxisMaxMajor(QwtPlot::yLeft, 6);
    plot->setAxisMaxMinor(QwtPlot::yLeft, 10);

    plot->setAxisScale(QwtPlot::xBottom, xm1, xm2);
    plot->setAxisScale(QwtPlot::yLeft, ym1, ym2);

    plot->setAxisTitle(QwtPlot::yLeft, tr("V (m/s)"));
    plot->setAxisTitle(QwtPlot::xBottom, tr("Tn (s)"));

    plot->canvas()->setFrameShape(QFrame::Box);
    plot->setCanvasBackground(Qt::white);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::SolidLine));
    grid->attach(plot);


    TripartiteGrid *grid2 = new TripartiteGrid();
    grid2->attach(plot);
    gGrid = grid2;
    grid2->enableXMin(true);
    grid2->setXAxisPos(0.18, 6.1);
    grid2->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
    grid2->setMinPen(QPen(Qt::gray, 0 , Qt::SolidLine));
    grid2->setUseG(true);
    grid2->setUseInch(false);
    setMinimumSize(700, 600);
    QTimer::singleShot(250, this, SLOT(plotgraph()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent * e)
{
    QWidget::resizeEvent(e);

    if (gGrid != 0)
    {
        gGrid->updateRange();
    }
}

void MainWindow::plotgraph()
{
    auto data = cvgk::get_data();
    int i = 0;
    foreach(auto v, data)
    {
        QwtPlotCurve* pc = new QwtPlotCurve();
        pc->setData(v);
        pc->attach(gPlot);
        QPen pn(gColors[i%4]);
        pn.setWidthF(1.5);
        pc->setPen(pn);
        ++i;
    }
    gPlot->replot();
}
