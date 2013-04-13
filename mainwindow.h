#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QResizeEvent>

class MainWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *);

public slots:
    void plotgraph();
};

#endif // MAINWINDOW_H
