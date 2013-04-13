#ifndef TRIPARTITEGRID_H
#define TRIPARTITEGRID_H

#include <qwt_plot_grid.h>
#include <qwt_scale_map.h>

class TripartiteGrid : public QwtPlotGrid
{
public:
    explicit TripartiteGrid();
    ~TripartiteGrid();

    virtual void draw( QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap,
                       const QRectF &rect ) const;
    /*!
     * \brief setXAxisPos
     * \param s1 first axis intersection with x-axis
     * \param s2 second axis intersection with y-axis
     */
    void setXAxisPos(double s1, double s2);

    virtual void updateScaleDiv(const QwtScaleDiv &xMap, const QwtScaleDiv &yMap );

    void setUseG(bool flag);
    void setUseInch(bool flag);
    void updateRange() const;

private:
    void drawLines( QPainter *painter, const QRectF &,
        Qt::Orientation orientation, const QwtScaleMap &,
        const QList<double> & ) const;
private:
    class PrivateImpl;
    PrivateImpl* m_impl;
};

#endif // TRIPARTITEGRID_H
