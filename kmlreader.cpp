#include <QtGui>
#include <QtXml>
#include <iostream>
#include <cmath>

#include "kmlreader.h"

#include "GeoDataLatLonBox.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"

KmlReader::KmlReader(QTreeWidget *tree)
{
    treeWidget = tree;
}

QMap <QString,GeoDataLineString> KmlReader::countryPolygon = QMap <QString,GeoDataLineString>();
//QMap <QString,GeoDataCoordinates> KmlReader::countryMiddle = QMap <QString,GeoDataCoordinates>();
//QMap <QString,GeoDataLineString> KmlReader::countryRectangle = QMap <QString,GeoDataLineString>();

bool
KmlReader::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    std::cerr << "Error: Cannot read file " << qPrintable(fileName)
              << ": " << qPrintable(file.errorString())
              << std::endl;
    return false;
  }

  reader.setDevice(&file);

  reader.readNext();
  while (!reader.atEnd()) {
  if (reader.isStartElement()) {
    if (reader.name() == "S_country") {
      readCountries();
    } else {
      reader.readNext();
    }
    } else {
      reader.readNext();
    }

  }

  file.close();
  if (reader.hasError()) {
    std::cerr << "Error: Failed to parse file "
              << qPrintable(fileName) << ": "
              << qPrintable(reader.errorString()) << std::endl;
    return false;
  } else if (file.error() != QFile::NoError) {
      std::cerr << "Error: Cannot read file " << qPrintable(fileName)
                << ": " << qPrintable(file.errorString())
                << std::endl;
      return false;
  }
  countryPolygon = getBiggerPolygons(countryBorder);
  QMapIterator<QString,GeoDataLineString> i(countryPolygon);
  /*while (i.hasNext()) {
    i.next();
    countryMiddle.insert(i.key(), getMiddle(i.value()));
    countryRectangle.insert(i.key(), getRectangle(i.value()));
  }
  */
  //QMapIterator<QString,GeoDataLineString> i(countryPolygon);
  return true;
}

void KmlReader::readCountries()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }
   //qDebug() << "readCountries" ;
        if (reader.isStartElement()) {
            if (reader.name() == "name") {
                readCountryName(treeWidget->invisibleRootItem());
            } else {
              skipUnknownElement();
            }
        } else {
            reader.readNext();
        }

    }
}

void KmlReader::readCountryName(QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    reader.readNext();
    item->setText(0, reader.text().toString());
    //countryBorder.insert(, 100);

    //reader.readNext();
    while (reader.name()!="Polygon" && reader.name()!="MultiGeometry") {
        reader.readNext();
    }
    if (reader.name()=="Polygon") {
      readPolygonElement(item);
     } else if(reader.name()=="MultiGeometry") {
      readMultiGeometry(item);
    }
       QFile file("out.txt");
       if (!file.open(QIODevice::QIODevice::Append | QIODevice::Text))
         return;
       QTextStream out(&file);
     if (item->child(0)->text(0)!="MultiGeometry") {
        countryBorder.insert(item->text(0), StringToPolygon(item->child(0)->text(1)));
        //qDebug()<< Qt::escape(item->child(0)->text(1));
        out << item->text(0)<<"\n";
        out<< item->child(0)->text(1)<<"\n";
        out<<"-------------------------------\n";
     } else if (item->child(0)->text(0)=="MultiGeometry"){
         for (int i=0; i < item->child(0)->childCount();++i){
           countryBorder.insert(item->text(0),StringToPolygon(item->child(0)->child(i)->text(1)) );
        //qDebug()<< item->parent()->text(0);
           //qDebug()<<item->text(0);
           //qDebug()<<item->child(0)->child(i)->text(1) ;
           out << item->text(0)<<"MULTI"<<"\n";
           out<< item->child(0)->child(i)->text(1)<<"\n";
           out<<"-------------------------------\n";
         }
      }
}

void
KmlReader::readPolygonElement(QTreeWidgetItem *parent)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
   while (reader.name()!="coordinates") {
        reader.readNext();
    }
  QString polygon = reader.readElementText(QXmlStreamReader::IncludeChildElements);
  //QXmlStreamReader pol_reader(polygon);
  item->setText(0,"Polygon");
  item->setText(1,polygon);
}

void KmlReader::readMultiGeometry(QTreeWidgetItem *parent)
{
       QTreeWidgetItem *item = new QTreeWidgetItem(parent);
       //QString mg = reader.readElementText(QXmlStreamReader::IncludeChildElements);
        //QXmlStreamReader pol_reader(polygon);
        reader.readNext();
       while (reader.name()!="MultiGeometry" ) {

         if ((reader.name()=="Polygon")&& reader.isStartElement()) {
         readPolygonElement(item);
         }
         reader.readNext();
       }
       item->setText(0,"MultiGeometry");
       //item->setText(1,mg);


}

void KmlReader::skipUnknownElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            skipUnknownElement();
        } else {
            reader.readNext();
        }
    }
}

GeoDataLineString
KmlReader::StringToPolygon(const QString & PolygonString)
{
  GeoDataLineString gdls (Tessellate);

  //QString ps = PolygonString;
 //ps = ps.replace(",0 "," ");
  //ps = ps.replace(",0\n","\n");
  QStringList list = PolygonString.split(" ", QString::SkipEmptyParts);
  for (int i = 0; i < list.size(); ++i) {
    //qDebug()<<list.value(i);
    bool ok;
   //QString temp = list.value(i).remove(list.value(i).lastIndexOf(",0"),2);
    //qDebug()<<temp;
     qreal lon=list.value(i).section(",",0,0).toDouble(&ok);
     qreal lat=list.value(i).section(",",1,1).toDouble(&ok);
      //GeoDataCoordinates temp(GeoDataCoordinates::fromString("0.0,1.0", ok));
      if(ok==false)
       qDebug()<<"0.0,1.0e-2";
      GeoDataCoordinates gd(lon, lat,0.0, GeoDataCoordinates::Degree);
      //qDebug()<<lon<<"  "<<lat;
      gdls.append(gd);
    /*if (ok==true){
      gdls->append(gd);
    } else {
      qDebug()<<list.value(i).section(",",0,0)<<"   "<<list.value(i).section(",",1,1);
      break;
    }*/
  }
  return gdls;
}
QMap <QString,GeoDataLineString>
KmlReader::getBiggerPolygons(const QMultiMap <QString,GeoDataLineString> & countryBorders)
{
  QMap  <QString,GeoDataLineString> countryPolygons;
  QList<QString> countries = countryBorders.uniqueKeys ();
  for (int i = 0; i < countries.size(); ++i) {
    if (countryBorders.count(countries.value(i))>1) {
        QVector <GeoDataLatLonAltBox> vectorBoxes(countryBorders.count(countries.value(i)));
        QVector <qreal> vectorBoxAreas(countryBorders.count(countries.value(i)));
        for (int k=0; k< countryBorders.count(countries.value(i)); ++k){
          vectorBoxes[k] = GeoDataLatLonAltBox::
                           fromLineString(countryBorders.values(countries.value(i)).value(k));
          vectorBoxAreas[k]= vectorBoxes[k].height()*vectorBoxes[k].width();
        }
        QVector <qreal> temp(countryBorders.count(countries.value(i)));
        temp = vectorBoxAreas;
        qSort(temp.begin(), temp.end(), qGreater<qreal>());
        int PolygonNumber = vectorBoxAreas.indexOf(temp.first());
        //qDebug()<<countries.value(i);
        countryPolygons.insert(countries.value(i),
                               countryBorders.values(countries.value(i)).value(PolygonNumber));
    } else {
        countryPolygons.insert(countries.value(i),
                               countryBorders.value(countries.value(i)));
      }
  }
return countryPolygons;
}

/*GeoDataCoordinates
KmlReader::getMiddle(const GeoDataLineString & gs)
{
  GeoDataCoordinates gc(0.0,0.0,0.0,GeoDataCoordinates::Degree);
  qreal mostEast;
  qreal mostWest;
  qreal mostNorth;
  qreal mostSouth;
  //qStableSort(temp.begin(), temp.end(), EastWestLessThan);
  QVector <qreal> lonValues(gs.size());
  QVector <qreal> latValues(gs.size());
  for(int i=0; i < gs.size() ; ++i){
    lonValues[i] = gs.at(i).longitude(GeoDataCoordinates::Degree);
    latValues[i] = gs.at(i).latitude(GeoDataCoordinates::Degree);
  }
  QVector <qreal> temp(gs.size());
  temp = lonValues;
  qSort(temp.begin(), temp.end(), qGreater<qreal>());
  mostEast = lonValues.at(lonValues.indexOf(temp.first()));
  mostWest = lonValues.at(lonValues.indexOf(temp.last()));
  temp = latValues;
  qSort(temp.begin(), temp.end(), qGreater<qreal>());
  mostNorth = latValues.value(latValues.indexOf(temp.first()));
  mostSouth = latValues.value(latValues.indexOf(temp.last()));
  gc.setLongitude((mostEast+mostWest)/2, GeoDataCoordinates::Degree);
  gc.setLatitude((mostNorth + mostSouth)/2, GeoDataCoordinates::Degree);
  return gc;
}
*/
/*GeoDataLineString
KmlReader::getRectangle(const GeoDataLineString & gs, const GeoDataCoordinates & middle)
{
  //GeoDataLatLonAltBox box = GeoDataLatLonAltBox::fromLineString(gs);
  GeoDataLinearRing polygon = GeoDataLinearRing(gs);
  polygon.setTessellate(true);


  qreal mostEast;
  qreal mostWest;
  qreal mostNorth;
  qreal mostSouth;
  QVector <qreal> lonValues(gs.size());
  QVector <qreal> latValues(gs.size());
  QVector<QPoint> PointVector(gs.size());
  for(int i=0; i < gs.size() ; ++i){
    lonValues[i] = gs.at(i).longitude(GeoDataCoordinates::Degree);
    latValues[i] = gs.at(i).latitude(GeoDataCoordinates::Degree);
    PointVector[i].setX(gs.at(i).longitude(GeoDataCoordinates::Degree)*1000);
    PointVector[i].setY(gs.at(i).latitude(GeoDataCoordinates::Degree)*1000);
  }
  QPolygon Polygon(PointVector);
  QRegion Region(Polygon);

  QVector<QRect> RectVector(Region.rectCount ());
  RectVector = Region.rects ();
  QVector <qreal> temp(gs.size());
  temp = lonValues;
  qSort(temp.begin(), temp.end(), qGreater<qreal>());
  mostEast = lonValues.at(lonValues.indexOf(temp.first()));
  mostWest = lonValues.at(lonValues.indexOf(temp.last()));
  temp = latValues;
  qSort(temp.begin(), temp.end(), qGreater<qreal>());
  mostNorth = latValues.value(latValues.indexOf(temp.first()));
  mostSouth = latValues.value(latValues.indexOf(temp.last()));
  qreal horStretch = abs(mostEast-mostWest)/10;
  qreal verStretch = abs(mostNorth-mostSouth)/10;

  qreal middleLat = middle.latitude(GeoDataCoordinates::Degree);
  qreal middleLon = middle.longitude(GeoDataCoordinates::Degree);
  //GeoDataCoordinates p1(middleLon-horStretch,middleLat+verStretch,0.0,GeoDataCoordinates::Degree,1000);
  GeoDataCoordinates p2(middleLon+horStretch,middleLat+verStretch,0.0,GeoDataCoordinates::Degree,1000);
  GeoDataCoordinates p3(middleLon+horStretch,middleLat-verStretch,0.0,GeoDataCoordinates::Degree,1000);
  GeoDataCoordinates p4(middleLon-horStretch,middleLat-verStretch,0.0,GeoDataCoordinates::Degree,1000);
  for (int i=0; i< 10;++i){
    if (p1.longitude(GeoDataCoordinates::Degree)>mostWest) {
      p1.setLongitude(p1.longitude(GeoDataCoordinates::Degree)-horStretch*i, GeoDataCoordinates::Degree);
      p4.setLongitude(p4.longitude(GeoDataCoordinates::Degree)-horStretch*i, GeoDataCoordinates::Degree);
      //qDebug()<<mostWest<< " " <<p1.longitude(GeoDataCoordinates::Degree)-horStretch*i;
    }
    if (p2.longitude(GeoDataCoordinates::Degree)<mostEast) {
      p2.setLongitude(p2.longitude(GeoDataCoordinates::Degree)+horStretch*i, GeoDataCoordinates::Degree);
      p3.setLongitude(p3.longitude(GeoDataCoordinates::Degree)+horStretch*i, GeoDataCoordinates::Degree);
    }
    if (p1.latitude(GeoDataCoordinates::Degree)<mostNorth) {
      p1.setLatitude(p1.latitude(GeoDataCoordinates::Degree)+verStretch*i, GeoDataCoordinates::Degree);
      p2.setLatitude(p2.latitude(GeoDataCoordinates::Degree)+verStretch*i, GeoDataCoordinates::Degree);
    }
    if (p3.latitude(GeoDataCoordinates::Degree)>mostSouth) {
      p3.setLatitude(p3.latitude(GeoDataCoordinates::Degree)-verStretch*i, GeoDataCoordinates::Degree);
      p4.setLatitude(p4.latitude(GeoDataCoordinates::Degree)-verStretch*i, GeoDataCoordinates::Degree);
    }
  }
  if (Region.boundingRect().width()*Region.boundingRect().height()>8000){

  QVector<int> AreaVector(RectVector.size());
  for(int i=0; i < AreaVector.size() ; ++i){
    AreaVector[i]=RectVector.at(i).width()*RectVector.at(i).height();
  }
  QVector<int> t(RectVector.size());
  t = AreaVector;
  qSort(t.begin(), t.end(), qGreater<int>());
  qDebug()<<t.first();
  QRectF Rect (RectVector.at(AreaVector.indexOf(t.first())));
  qDebug()<<AreaVector.indexOf(t.first());
  GeoDataCoordinates p1 (Rect.topLeft().x()/1000.0 , Rect.topLeft().y()/1000.0,0.0,GeoDataCoordinates::Degree);
  qDebug()<<p1.latitude();
  GeoDataCoordinates p2(Rect.topRight().x()/1000.0, Rect.topRight().y()/1000.0,0.0,GeoDataCoordinates::Degree,1000);
  qDebug()<<p2.latitude();
  GeoDataCoordinates p3(Rect.bottomRight().x()/1000.0 , Rect.bottomRight().y()/1000.0,0.0,GeoDataCoordinates::Degree,1000);
  qDebug()<<p3.latitude();
  GeoDataCoordinates p4(Rect.bottomLeft().x()/1000.0, Rect.bottomLeft().y()/1000.0,0.0,GeoDataCoordinates::Degree,1000);
  qDebug()<<p4.latitude();
  GeoDataLineString LinString (Tessellate);
  LinString.append(p1);
  LinString.append(p2);
  LinString.append(p3);
  LinString.append(p4);
  return LinString;
  } else{
  GeoDataCoordinates p1(middleLon-horStretch,middleLat+verStretch,0.0,GeoDataCoordinates::Degree,1000);
  GeoDataCoordinates p2(middleLon+horStretch,middleLat+verStretch,0.0,GeoDataCoordinates::Degree,1000);
  GeoDataCoordinates p3(middleLon+horStretch,middleLat-verStretch,0.0,GeoDataCoordinates::Degree,1000);
  GeoDataCoordinates p4(middleLon-horStretch,middleLat-verStretch,0.0,GeoDataCoordinates::Degree,1000);
  GeoDataLineString LinString (Tessellate);
  LinString.append(p1);
  LinString.append(p2);
  LinString.append(p3);
  LinString.append(p4);
  return LinString;
   }
}

GeoDataLineString
KmlReader::getRectangle(const GeoDataLineString & gs)
{
  QVector<GeoDataLatLonAltBox> BoxVector(0);
  GeoDataLatLonBox Box = GeoDataLatLonBox::fromLineString(gs);
  qreal East = Box.east(GeoDataCoordinates::Degree);
  qreal West = Box.west(GeoDataCoordinates::Degree);
  qreal North = Box.north(GeoDataCoordinates::Degree);
  qreal South = Box.south(GeoDataCoordinates::Degree);
  qreal Quant = 100.0;
  qreal horStretch = abs(East-West)/Quant;
  qreal verStretch = abs(North-South)/Quant;
  GeoDataLinearRing LinearRing(gs);
  const GeoDataGeometry gg(gs);
  GeoDataPolygon Polygon(gg);
  //qDebug()<<LinearRing.contains(GeoDataCoordinates(1.0,0.0,0.0));
  for (int i=1; i<=Quant;++i){
    for (int j=1;j<=Quant;++j){
      GeoDataCoordinates GPoint(West+horStretch*j,South+verStretch*i,0.0,GeoDataCoordinates::Degree);
      //qDebug()<< GPoint.latitude()<<" "<<i<<"line 388";
      //qDebug()<< GPoint.longitude()<<" "<<j<<"line 389";
      if (LinearRing.contains(GPoint)){
        //qDebug()<<LinearRing.contains(GPoint) <<GPoint.latitude(GeoDataCoordinates::Degree)<<GPoint.longitude(GeoDataCoordinates::Degree)<<"line 391";
        for (int k=1; k<=Quant;++k){
          for (int l=1;l<=Quant;++l){
            GeoDataLatLonBox TempBox(GPoint.latitude()+verStretch*k,GPoint.latitude(),
              GPoint.longitude()+horStretch*l,GPoint.longitude(),GeoDataCoordinates::Degree);
              //qDebug()<< TempBox.east(GeoDataCoordinates::Degree)<<"line 394";
            if (!crosses(LinearRing, TempBox)) {
               BoxVector.append(TempBox);
               qDebug()<< TempBox.east(GeoDataCoordinates::Degree)<<"line 403";
            }
          }
        }
      } else {
        continue;
      }
    }
  }
  if (BoxVector.size()>0){
    QVector<qreal> AreaVector(BoxVector.size());
    for (int i=0; i< BoxVector.size();++i){
      AreaVector[i]=BoxVector.at(i).height()*BoxVector.at(i).width();
    }

    QVector<qreal> TempAreaVector(BoxVector.size());
    TempAreaVector = AreaVector;
    qSort(TempAreaVector.begin(), TempAreaVector.end(), qGreater<qreal>());
    GeoDataLatLonBox LargestBox = BoxVector.at(AreaVector.indexOf(TempAreaVector.first()));
    BoxVector.resize(0);
    East = LargestBox.east(GeoDataCoordinates::Degree);
    West = LargestBox.west(GeoDataCoordinates::Degree);
    North = LargestBox.north(GeoDataCoordinates::Degree);
    South = LargestBox.south(GeoDataCoordinates::Degree);
    GeoDataCoordinates p1(West,North, 0.0, GeoDataCoordinates::Degree);
    GeoDataCoordinates p2(East,North, 0.0, GeoDataCoordinates::Degree);
    GeoDataCoordinates p3(East,South, 0.0, GeoDataCoordinates::Degree);
    GeoDataCoordinates p4(West,South, 0.0, GeoDataCoordinates::Degree);
    GeoDataLineString LinString (FollowGround );
    LinString.append(p1);
    LinString.append(p2);
    LinString.append(p3);
    LinString.append(p4);
    return LinString;
  } else {
    return gs;
  }
}

bool
KmlReader::crosses(const GeoDataLinearRing & LineString, const GeoDataLatLonBox & Box)
{
  for (int i=0;i<LineString.size();++i){
    if (Box.contains(LineString.at(i)))
      qDebug()<< LineString.at(i).longitude(GeoDataCoordinates::Degree);
      return true;
  }
  return false;
}*/
