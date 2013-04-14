#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>

class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = 0);
    ~PlotWidget();

protected:
    void resizeEvent(QResizeEvent *);

public slots:
    void plotgraph();
    
};

#endif // PLOTWIDGET_H
