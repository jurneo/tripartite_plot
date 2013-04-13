#ifndef READER_H
#define READER_H

#include <qwt_series_data.h>
#include <qvector>

namespace cvgk {

extern void read();
extern QVector<QwtPointArrayData* > get_data();

}

#endif // READER_H
