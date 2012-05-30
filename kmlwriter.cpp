#include "kmlwriter.h"
#include "kmlreader.h"
#include <QFile>

#include "GeoDataLatLonBox.h"
#include "GeoDataCoordinates.h"

KmlWriter::KmlWriter(QMap <QString,GeoDataLineString> & countryPolygon, QString &FileName)
{
  QFile File(FileName);
  if (!File.open(QIODevice::QIODevice::Append | QIODevice::Text))
    return;
  QTextStream out(&File);
  out.setCodec("UTF-8");
  //out << "<?xml version="1.0" encoding="UTF-8"?>\n";
  //out << "<kml<<"Qt::escape("xmlns="http://earth.google.com/kml/2.0">\n";
  out << "<Document>\n";
  out << "  <name>Countries</name>\n";
  out << "  <visibility>0</visibility>\n";
  //out << "  <Schema parent="Placemark" name="S_country">\n";
  out << "  </Schema>\n";
  out << "  <Folder>\n";
  out << "    <name>Countries</name>\n";
  out << "    <visibility>0</visibility>\n";
  writeFile(KmlReader::countryPolygon, out );
  out << "  </Folder>\n";
  out << " </Document>\n";
  out << "</kml>\n";
}

bool
KmlWriter::writeFile(QMap <QString,GeoDataLineString> & countryPolygon, QTextStream & out)
{
  QMapIterator<QString,GeoDataLineString> i(countryPolygon);
  while (i.hasNext()) {
    i.next();
    out << "      <S_country>\n";
    out << "        <name>" << i.key() << "</name>\n";
    out << "        <visibility>0</visibility>\n";
    /*out << "          <Polygon>\n";
    out << "            <outerBoundaryIs>\n";
    out << "              <LinearRing>\n";
    out << "                <coordinates>\n";
    GeoDataLatLonBox Box = GeoDataLatLonBox::fromLineString(i.value());
    GeoDataCoordinates p1 (Box.east(GeoDataCoordinates::Degree), Box.north(GeoDataCoordinates::Degree), 0.0,GeoDataCoordinates::Degree);
    GeoDataCoordinates p2 (Box.west(GeoDataCoordinates::Degree), Box.north(GeoDataCoordinates::Degree), 0.0,GeoDataCoordinates::Degree);
    */
    GeoDataLatLonBox Box = GeoDataLatLonBox::fromLineString(i.value());
    out << "        <LatLonBox>\n";
    out << "          <north>" << Box.north(GeoDataCoordinates::Degree) << "</north>\n";
    out << "          <south>" << Box.south(GeoDataCoordinates::Degree) << "</south>\n";
    out << "          <east>" <<  Box.east(GeoDataCoordinates::Degree) << "</east>\n";
    out << "          <west>" <<  Box.east(GeoDataCoordinates::Degree) << "</west>\n";
    out << "        </LatLonBox>\n";
    out << "      </S_country>\n";
  }
return true;
}


