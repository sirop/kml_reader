#ifndef KMLREADER_H
#define KMLREADER_H

#include <QXmlStreamReader>

#include <GeoDataLineString.h>
#include <GeoDataLinearRing.h>

using namespace Marble;

class QTreeWidget;
class QTreeWidgetItem;

class KmlReader
{
public:
  KmlReader(QTreeWidget *tree);
  static QMap <QString,GeoDataLineString> countryPolygon;
  //static QMap <QString,GeoDataCoordinates> countryMiddle;
  //static QMap <QString,GeoDataLineString> countryRectangle;

  bool readFile(const QString &fileName);

friend class MyMarbleWidget;
private:
  void readCountries();
  void readCountryName(QTreeWidgetItem *parent);
  void readPolygonElement(QTreeWidgetItem *parent);
  void readMultiGeometry(QTreeWidgetItem *parent);
  void skipUnknownElement();
  GeoDataLineString StringToPolygon(const QString & PolygonString);
 // GeoDataCoordinates getMiddle(const GeoDataLineString & gs);
 // GeoDataLineString  getRectangle(const GeoDataLineString & gs, const GeoDataCoordinates & middle);
 // GeoDataLineString  getRectangle(const GeoDataLineString & gs);

  QTreeWidget *treeWidget;
  QXmlStreamReader reader;
  QMultiMap <QString,GeoDataLineString> countryBorder;
  QMap<QString,GeoDataLineString> getBiggerPolygons(const QMultiMap<QString,GeoDataLineString> & countryBorders);
  // bool crosses(const GeoDataLinearRing & LineString, const GeoDataLatLonBox & Box);

};

#endif
