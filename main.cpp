
#include <QtGui>
#include <iostream>

#include "kmlreader.h"

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
    GeoDataCoordinates  p1 (10.47124,46.87135,0.0, GeoDataCoordinates::Degree);
    GeoDataCoordinates  p2(20.39076,47.00257,0.0, GeoDataCoordinates::Degree  );
    GeoDataCoordinates  p3(15.39076,60.00257,0.0, GeoDataCoordinates::Degree  );
    GeoDataLineString gdls(Tessellate);
    gdls.append(p1);
    gdls.append(p2);
    gdls.append(p3);
    painter->setPen(Qt::green);
    painter->drawEllipse(p1, 20, 20);
    painter->setPen(Qt::black);
    painter->drawLine (p1,p2 ,true );
    //GeoDataGeometry gdp(gdls);
    //GeoDataLinearRing gdlr(gdp);
    painter->drawPolygon(gdls, Qt::OddEvenFill);
   QMap <QString,GeoDataLineString> cp = KmlReader::countryPolygon;
  foreach (GeoDataLineString value, cp)
    painter->drawPolygon(value, Qt::OddEvenFill);
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

    MyMarbleWidget *myMap=new MyMarbleWidget();
    myMap->setMapThemeId("earth/plain/plain.dgml");

    // Marble::GeoDataLineString gdlr;


   /* gdlr.append(p1);
    gdlr.append(p2);
    gp->drawPolygon(gdlr);*/
    GeoPainter *gp=new GeoPainter(myMap, myMap->viewport(),NormalQuality, false);
    gp->begin(myMap);
    myMap->customPaint(gp);
    gp->end();
    myMap->show();
    return app.exec();
}
