#ifndef KMLREADER_H
#define KMLREADER_H

#include <QXmlStreamReader>

#include <GeoDataLineString.h>

using namespace Marble;

class QTreeWidget;
class QTreeWidgetItem;

class KmlReader
{
public:
  KmlReader(QTreeWidget *tree);
  static QMap <QString,GeoDataLineString> countryPolygon;

  bool readFile(const QString &fileName);

friend class MyMarbleWidget;
private:
  void readCountries();
  void readCountryName(QTreeWidgetItem *parent);
  void readPolygonElement(QTreeWidgetItem *parent);
  void readMultiGeometry(QTreeWidgetItem *parent);
  void skipUnknownElement();
  GeoDataLineString StringToPolygon(const QString & PolygonString);


  QTreeWidget *treeWidget;
  QXmlStreamReader reader;
  QMultiMap <QString,GeoDataLineString> countryBorder;
  QMap<QString,GeoDataLineString> flatMap(const QMultiMap<QString,GeoDataLineString> & countryBorders);

};

#endif
