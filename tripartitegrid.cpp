#include "tripartitegrid.h"
#include <qtgui>
#include "qwt_painter.h"
#include "qwt_text.h"
#include "qwt_scale_map.h"
#include "qwt_scale_div.h"
#include "qwt_scale_draw.h"
#include "qwt_thermo.h"
#include "qwt_math.h"
#include "qwt_plot.h"
#include <qpainter.h>
#include <qpen.h>
#include <qpalette>
#include "qwt_scale_engine.h"
#include <iostream>
#include <cmath>

namespace {
const double INCH2G = 0.00259007918;
const double G = 9.81;
}

class TripartiteGrid::PrivateImpl
{
public:
    PrivateImpl()
        : s1(0.0), s2(0.0), t2(0.0)
        , angle(-45.0)
        , isInch(false)
        , useG(false)
    {
        xd.setTransformation(new QwtScaleTransformation(QwtScaleTransformation::Log10));
        yd.setTransformation(new QwtScaleTransformation(QwtScaleTransformation::Log10));
        yd.setAlignment(QwtScaleDraw::RightScale);
    }

    double s1;
    double s2;
    double t2;
    double angle;
    bool isInch;
    bool useG;

    QwtScaleDraw xd;
    QwtScaleDraw yd;
};

TripartiteGrid::TripartiteGrid() :
    QwtPlotGrid(),
    m_impl(new PrivateImpl)
{
}

TripartiteGrid::~TripartiteGrid()
{
    delete m_impl;
}

void TripartiteGrid::drawLines( QPainter *painter, const QRectF &canvasRect,
    Qt::Orientation orientation, const QwtScaleMap &scaleMap,
    const QList<double> &values ) const
{
    const double x1 = canvasRect.left();
    const double x2 = canvasRect.right() - 1.0;
    const double y1 = canvasRect.top();
    const double y2 = canvasRect.bottom() - 1.0;

    const bool doAlign = QwtPainter::roundingAlignment( painter );

    for ( uint i = 0; i < ( uint )values.count(); i++ )
    {
        double value = scaleMap.transform( values[i] );
        if ( doAlign )
            value = qRound( value );

        if ( orientation == Qt::Horizontal )
        {
            if ( qwtFuzzyGreaterOrEqual( value, y1 ) &&
                qwtFuzzyLessOrEqual( value, y2 ) )
            {
                QwtPainter::drawLine( painter, x1, value, x2, value );
            }
        }
        else
        {
            if ( qwtFuzzyGreaterOrEqual( value, x1 ) &&
                qwtFuzzyLessOrEqual( value, x2 ) )
            {
                QwtPainter::drawLine( painter, value, y1, value, y2 );
            }
        }
    }
}


/*!
  \brief Draw the grid

  The grid is drawn into the bounding rectangle such that
  gridlines begin and end at the rectangle's borders. The X and Y
  maps are used to map the scale divisions into the drawing region
  screen.
  \param painter  Painter
  \param xMap X axis map
  \param yMap Y axis
  \param canvasRect Contents rect of the plot canvas
*/
void TripartiteGrid::draw( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &canvasRect ) const
{
    //  draw minor gridlines
    QPen minPen = this->minPen();
    minPen.setCapStyle( Qt::FlatCap );

    painter->setPen( minPen );

    if ( xEnabled() && xMinEnabled() )
    {
        drawLines( painter, canvasRect, Qt::Vertical, xMap,
            xScaleDiv().ticks( QwtScaleDiv::MinorTick ) );
        drawLines( painter, canvasRect, Qt::Vertical, xMap,
            xScaleDiv().ticks( QwtScaleDiv::MediumTick ) );
    }

    if ( yEnabled() && yMinEnabled() )
    {
        drawLines( painter, canvasRect, Qt::Horizontal, yMap,
            yScaleDiv().ticks( QwtScaleDiv::MinorTick ) );
        drawLines( painter, canvasRect, Qt::Horizontal, yMap,
            yScaleDiv().ticks( QwtScaleDiv::MediumTick ) );
    }

    //  draw major gridlines
    QPen majPen = this->majPen();
    majPen.setCapStyle( Qt::FlatCap );

    painter->setPen( majPen );

    if ( xEnabled() )
    {
        drawLines( painter, canvasRect, Qt::Vertical, xMap,
            xScaleDiv().ticks( QwtScaleDiv::MajorTick ) );
    }

    if ( yEnabled() )
    {
        drawLines( painter, canvasRect, Qt::Horizontal, yMap,
            yScaleDiv().ticks( QwtScaleDiv::MajorTick ) );
    }

    auto xpos = xScaleDiv().interval().minValue();
    auto ypos = yScaleDiv().interval().minValue();

    QPen pn(Qt::gray, 0, Qt::SolidLine);
    QPalette pl(pn.color());
    auto oldPen = painter->pen();
    painter->setPen(pn);

    painter->save();
    painter->translate(xMap.transform(m_impl->s1), yMap.transform(ypos));
    painter->rotate(m_impl->angle);
    m_impl->xd.draw(painter, pl);
    painter->translate(100,0);
    QPen blackPen(Qt::black);
    painter->setPen(blackPen);
    painter->drawText(0, -15, "Disp. (m)");
    painter->restore();

    painter->save();
    painter->translate(xMap.transform(xpos), yMap.transform(m_impl->t2));
    painter->rotate(m_impl->angle);
    m_impl->yd.draw(painter, pl);

    painter->translate(-30, 100);
    painter->rotate(90);
    painter->setPen(blackPen);
    painter->drawText(0, -15, "Accel. (g)");
    painter->restore();

    painter->setPen(oldPen);
}

void TripartiteGrid::setXAxisPos(double s1, double s2)
{
    m_impl->s1 = s1;
    m_impl->s2 = s2;
}

void TripartiteGrid::updateRange() const
{
    if (plot() == 0)
    {
        return;
    }
    auto s1 = m_impl->s1;
    auto s2 = m_impl->s2;

    auto& sy = plot()->axisScaleDraw(QwtPlot::yLeft)->scaleMap();
    auto& sx = plot()->axisScaleDraw(QwtPlot::xBottom)->scaleMap();
    auto* xScaleDiv = plot()->axisScaleDiv(QwtPlot::xBottom);
    auto* yScaleDiv = plot()->axisScaleDiv(QwtPlot::yLeft);

    auto x1 = sx.transform(s1);
    auto y1 = sy.transform(yScaleDiv->interval().minValue());
    auto x2 = sx.transform(xScaleDiv->interval().maxValue());
    auto len1 = std::fabs(x2-x1);
    auto y2 = y1-len1;

    // D = V * Tn / 2pi
    auto d1 = s1 * yScaleDiv->interval().minValue() / (2*M_PI);
    auto d2 = sy.invTransform(y2) * xScaleDiv->interval().maxValue() / (2*M_PI);

    QwtLog10ScaleEngine eg;
    auto xs = eg.divideScale(d1, d2, 10, 10);
    auto len = std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    m_impl->xd.setLength(len);
    m_impl->xd.setScaleDiv(xs);
    m_impl->angle = atan((y2-y1)/(x2-x1))*180.0/M_PI;

    x1 = sx.transform(s2);
    x2 = sx.transform(xScaleDiv->interval().minValue());
    len1 = std::fabs(x2-x1);
    y2 = y1-len1;

    m_impl->t2 = sy.invTransform(y2);

    // A = V * 2pi / Tn
    double factor = 1;
    if (m_impl->useG)
    {
        factor = m_impl->isInch ? INCH2G : G;
    }
    d1 = yScaleDiv->interval().minValue() * (2*M_PI) / s2 * factor;
    d2 = m_impl->t2 *  (2*M_PI) / xScaleDiv->interval().minValue() * factor;
    auto ys = eg.divideScale(d1, d2, 10, 10);
    m_impl->yd.setScaleDiv(ys);
    len = std::sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    m_impl->yd.setLength(len);
}

void TripartiteGrid::updateScaleDiv(const QwtScaleDiv &xMap, const QwtScaleDiv &yMap )
{
    QwtPlotGrid::updateScaleDiv(xMap, yMap);

    updateRange();
}

void TripartiteGrid::setUseG(bool flag)
{
    m_impl->isInch = flag;
}

void TripartiteGrid::setUseInch(bool flag)
{
    m_impl->useG = flag;
}
