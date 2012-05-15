#include <QtGui>
#include <QtXml>
#include <iostream>

#include "kmlreader.h"

#include "GeoDataLatLonAltBox.h"

KmlReader::KmlReader(QTreeWidget *tree)
{
    treeWidget = tree;
}

 QMap <QString,GeoDataLineString> KmlReader::countryPolygon=QMap <QString,GeoDataLineString>();
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
  countryPolygon = flatMap(countryBorder);
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
KmlReader::flatMap(const QMultiMap <QString,GeoDataLineString> & countryBorders)
{
  QMap  <QString,GeoDataLineString> countryPolygons;
  QList<QString> countries = countryBorders.uniqueKeys ();
  for (int i = 0; i < countries.size(); ++i) {
    if (countryBorders.count(countries.value(i))>1) {
        QVector <GeoDataLatLonAltBox> vectorBoxes(countryBorders.count(countries.value(i)));
        QVector <qreal> vectorBoxAreas(countryBorders.count(countries.value(i)));
        for (int k=0; k< countryBorders.count(countries.value(i)); ++k){
          vectorBoxes[k] = GeoDataLatLonAltBox::fromLineString(countryBorders.values(countries.value(i)).value(k));
          vectorBoxAreas[k]= vectorBoxes[k].height()*vectorBoxes[k].width();
        }
        QVector <qreal> temp(countryBorders.count(countries.value(i)));
        temp = vectorBoxAreas;
        qSort(temp.begin(), temp.end(), qGreater<qreal>());
        int PolNumber = vectorBoxAreas.indexOf(temp.first());
        qDebug()<<countries.value(i);
        countryPolygons.insert(countries.value(i),
                               countryBorders.values(countries.value(i)).value(PolNumber));
    } else {
        countryPolygons.insert(countries.value(i),
                               countryBorders.value(countries.value(i)));
      }
  }
return countryPolygons;
}
