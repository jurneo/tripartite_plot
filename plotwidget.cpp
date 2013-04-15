#include "plotwidget.h"

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
#include <qsettings.h>

namespace {
TripartiteGrid* gGrid = 0;
QwtPlot* gPlot = 0;
QColor gColors[] = { Qt::blue, Qt::red, Qt::green, Qt::black };
}

PlotWidget::PlotWidget(QWidget *parent) :
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
    v->setContentsMargins(0,0,0,0);
    v->addWidget(plot);
    setContentsMargins(0,0,0,0);

    QSettings setting("setting.ini", QSettings::IniFormat);
    setting.beginGroup("axis-range");
    auto xm1 = setting.value("xm1", 0.03).toDouble();
    auto xm2 = setting.value("xm2", 20).toDouble();
    auto ym1 = setting.value("ym1", 0.03).toDouble();
    auto ym2 = setting.value("ym2", 20).toDouble();
    setting.endGroup();

    plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);

    plot->setAxisMaxMajor(QwtPlot::xBottom, 6);
    plot->setAxisMaxMinor(QwtPlot::xBottom, 10);
    plot->setAxisMaxMajor(QwtPlot::yLeft, 6);
    plot->setAxisMaxMinor(QwtPlot::yLeft, 10);

    plot->setAxisScale(QwtPlot::xBottom, xm1, xm2);
    plot->setAxisScale(QwtPlot::yLeft, ym1, ym2);

    setting.beginGroup("axis-text");
    QString veloText = setting.value("veloText", "V (m/s)").toString();
    QString timeText = setting.value("timeText", "Tn (s)").toString();
    QString dispText = setting.value("dispText", "Disp. (m)").toString();
    QString accelText = setting.value("accelText", "Accel. (g)").toString();
    setting.endGroup();

    plot->setAxisTitle(QwtPlot::yLeft, veloText);
    plot->setAxisTitle(QwtPlot::xBottom, timeText);

    plot->canvas()->setFrameShape(QFrame::Box);
    plot->setCanvasBackground(Qt::white);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMajPen(QPen(Qt::gray, 0, Qt::SolidLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::SolidLine));
    grid->attach(plot);

    setting.beginGroup("xpos");
    auto xpos1 = setting.value("xpos1", 0.18).toDouble();
    auto xpos2 = setting.value("xpos2", 6.1).toDouble();
    setting.endGroup();

    TripartiteGrid *grid2 = new TripartiteGrid();
    grid2->attach(plot);
    gGrid = grid2;
    grid2->enableXMin(true);
    grid2->enableYMin(true);
    grid2->setXAxisPos(xpos1, xpos2);
    grid2->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));
    grid2->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid2->setDisplacementText(dispText);
    grid2->setAccelerationText(accelText);

    setting.beginGroup("unit");
    QString unit = setting.value("unit","m").toString();
    setting.endGroup();
    unit = unit.toLower();
    if (unit == "inch")
    {
        grid2->setUnit(cvgk::inch);
    }
    else if (unit == "m")
    {
        grid2->setUnit(cvgk::m);
    }
    else
    {
        grid2->setUnit(cvgk::cm);
    }
    setMinimumSize(700, 600);

    QTimer::singleShot(250, this, SLOT(plotgraph()));

    setting.beginGroup("axis-range");
    setting.setValue("xm1", xm1);
    setting.setValue("xm2", xm2);
    setting.setValue("ym1", ym1);
    setting.setValue("ym2", ym2);
    setting.endGroup();
    setting.beginGroup("axis-text");
    setting.setValue("veloText", veloText);
    setting.setValue("timeText", timeText);
    setting.setValue("dispText", dispText);
    setting.setValue("accelText", accelText);
    setting.endGroup();
    setting.beginGroup("xpos");
    setting.setValue("xpos1", xpos1);
    setting.setValue("xpos2", xpos2);
    setting.endGroup();
    setting.beginGroup("unit");
    setting.setValue("unit",unit);
    setting.endGroup();
}

PlotWidget::~PlotWidget()
{
}

void PlotWidget::resizeEvent(QResizeEvent * e)
{
    QWidget::resizeEvent(e);

    if (gGrid != 0)
    {
        gGrid->updateRange();
    }
}

void PlotWidget::plotgraph()
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
