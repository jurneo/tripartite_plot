#include "reader.h"
#include <qstring>
#include <qfile>
#include <qdir>
#include <qfiledialog>
#include <qdebug>
#include <iostream>

namespace {
QVector<QwtPointArrayData* > gdata;
}

namespace cvgk {

void read()
{
    gdata.clear(); // may memory leak

    QStringList filters;
    filters << "*.txt";
    QDir dir = QDir::currentPath();
    dir.cd("data");
    dir.setNameFilters(filters);
    foreach(QString f, dir.entryList())
    {
        QFile fl(dir.absoluteFilePath(f));
        if (fl.open(QIODevice::ReadOnly))
        {
            QVector<double> xv;
            QVector<double> yv;

            QTextStream in(&fl);
            int i = 0;
            while (!in.atEnd())
            {
                auto line = in.readLine();
                ++i;
                if (i<=1)
                {
                    continue;
                }
                auto list = line.split(" ");
                if (list.count()<2)
                {
                    continue;
                }
                //qDebug() << list.at(0) << ", " << list.at(1);
                xv << list.at(0).toDouble();
                yv << list.at(1).toDouble();
            }

            if (xv.empty())
            {
                continue;
            }

            QwtPointArrayData* pw = new QwtPointArrayData(xv,yv);
            gdata.push_back(pw);
        }
    }

    std::cout << "total size: "; qDebug() << gdata.size();
}

QVector<QwtPointArrayData* > get_data()
{
    return gdata;
}

}
