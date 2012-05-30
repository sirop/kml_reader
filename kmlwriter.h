#ifndef KMLWRITER_H
#define KMLWRITER_H

#include <QTextStream>
#include <GeoDataLineString.h>

using namespace Marble;

class KmlWriter
{
public:
  KmlWriter(QMap <QString,GeoDataLineString> & countryPolygon, QString &FileName);
  bool writeFile(QMap <QString,GeoDataLineString> & countryPolygon, QTextStream & out);
};

#endif
