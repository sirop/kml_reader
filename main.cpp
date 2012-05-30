
#include <QtGui>
#include <iostream>

#include "kmlreader.h"
#include "kmlwriter.h"

#include <MarbleWidget.h>
#include <GeoPainter.h>
#include <GeoDataLineString.h>
#include <GeoDataLinearRing.h>
#include <GeoDataCoordinates.h>

using namespace Marble;
class MyMarbleWidget : public MarbleWidget
{
public:
  virtual void customPaint(GeoPainter* painter);
  friend class KmlReader;
};

void MyMarbleWidget::customPaint(GeoPainter* painter)
{
  painter->setPen(Qt::green);
    //painter->drawEllipse(p1, 20, 20);
    //painter->setPen(Qt::black);
    //painter->drawLine (p1,p2 ,true );
    //GeoDataGeometry gdp(gdls);
    //GeoDataLinearRing gdlr(gdp);
    //painter->drawPolygon(gdls, Qt::OddEvenFill);
   //QMap <QString,GeoDataLineString> cp = KmlReader::countryPolygon;
  foreach (GeoDataLineString value, KmlReader::countryPolygon)
    painter->drawPolygon(value, Qt::OddEvenFill);
  //foreach (GeoDataCoordinates value, KmlReader::countryMiddle){
    //painter->drawEllipse(value, 10,10);
    //qDebug()<<value.latitude();
  //}
  /*painter->setPen(Qt::blue);
  foreach (GeoDataLineString value, KmlReader::countryRectangle){
  painter->drawPolygon(value, Qt::OddEvenFill);
  GeoDataLatLonBox Box = GeoDataLatLonBox::fromLineString(value);
  //const QRectF Rect(  GeoDataCoordinates(Box.west(), Box.north(), Box.width(), Box.height() );
  qreal East = Box.east(GeoDataCoordinates::Degree);
  qreal West = Box.west(GeoDataCoordinates::Degree);
  qreal North = Box.north(GeoDataCoordinates::Degree);
  qreal South = Box.south(GeoDataCoordinates::Degree);
  const GeoDataCoordinates gc((East-West)/2+West, (North-South)/2+South, 0.0,GeoDataCoordinates::Degree);
  painter->drawRect(gc,Box.width(GeoDataCoordinates::Degree),Box.height(GeoDataCoordinates::Degree),true);
  GeoDataLinearRing LinearRing(value);
  if (LinearRing.contains(gc)){
      qDebug()<<gc.latitude(GeoDataCoordinates::Degree);
    }
  }
  */
}


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QStringList args = QApplication::arguments();

  if (args.count() < 2) {
    std::cerr << "Usage: xmlstreamreader file1.xml..."
              << std::endl;
    return 1;
  }

  QStringList labels;
  labels << QObject::tr("Terms") << QObject::tr("Pages");

  QTreeWidget treeWidget;
  treeWidget.setHeaderLabels(labels);
  treeWidget.header()->setResizeMode(QHeaderView::Interactive);
  treeWidget.setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  treeWidget.setWindowTitle(QObject::tr("XML Stream Reader"));
  treeWidget.show();

  KmlReader reader(&treeWidget);
  for (int i = 1; i < args.count(); ++i)
    reader.readFile(args[i]);
  QString KmlFile("boxes.kml");
  KmlWriter writer(KmlReader::countryPolygon, KmlFile);

  MyMarbleWidget *myMap=new MyMarbleWidget();
  myMap->setMapThemeId("earth/plain/plain.dgml");

  GeoPainter *gp=new GeoPainter(myMap, myMap->viewport(),NormalQuality, false);
  gp->begin(myMap);
  myMap->customPaint(gp);
  gp->end();
  myMap->show();
  return app.exec();
}
