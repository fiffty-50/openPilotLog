/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "calc.h"
// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

/*!
 * \brief Calc::blocktime Calculates Block Time for a given departure and arrival time
 * \param tofb QTime Time Off Blocks
 * \param tonb QTime Time On Blocks
 * \return Block Time in minutes
 */
QTime Calc::blocktime(QTime tofb, QTime tonb)
{
    if (tonb > tofb) { // landing same day
        QTime blocktimeout(0, 0); // initialise return value at midnight
        int blockseconds = tofb.secsTo(tonb); // returns seconds between 2 time objects
        blocktimeout = blocktimeout.addSecs(blockseconds);
        return blocktimeout;

    } else { // landing next day
        QTime midnight(0, 0);
        QTime blocktimeout(0, 0); // initialise return value at midnight
        int blockseconds = tofb.secsTo(midnight); // returns seconds passed until midnight
        blocktimeout = blocktimeout.addSecs(blockseconds);
        blockseconds = midnight.secsTo(tonb); // returns seconds passed after midnight
        blocktimeout = blocktimeout.addSecs(blockseconds);
        return blocktimeout;
    }
}


/*!
 * \brief Calc::minutes_to_string Converts database time to String Time
 * \param blockminutes int from database
 * \return String hh:mm
 */
QString Calc::minutesToString(QString blockminutes)
{
    int minutes = blockminutes.toInt();
    QString hour = QString::number(minutes / 60);
    if (hour.size() < 2) {
        hour.prepend("0");
    }
    QString minute = QString::number(minutes % 60);
    if (minute.size() < 2) {
        minute.prepend("0");
    }
    QString blocktime = hour + ":" + minute;
    return blocktime;
};

/*!
 * \brief Calc::time_to_minutes converts QTime to int minutes
 * \param time QTime
 * \return int time as number of minutes
 */
int Calc::qTimeToMinutes(QTime time)
{
    QString timestring = time.toString("hh:mm");
    int minutes = (timestring.left(2).toInt()) * 60;
    minutes += timestring.right(2).toInt();
    return minutes;
}

/*!
 * \brief Calc::string_to_minutes Converts String Time to String Number of Minutes
 * \param timestring "hh:mm"
 * \return String number of minutes
 */
int Calc::stringToMinutes(QString timestring)
{
    int minutes = (timestring.left(2).toInt()) * 60;
    minutes += timestring.right(2).toInt();
    timestring = QString::number(minutes);
    return minutes;
}

/*!
 * The purpose of the following functions is to provide functionality enabling the Calculation of
 * night flying time. EASA defines night as follows:
 *
 * ‘Night’  means  the  period  between  the  end  of  evening  civil  twilight  and  the  beginning  of
 * morning  civil  twilight  or  such  other  period  between  sunset  and  sunrise  as  may  be  prescribed
 * by  the  appropriate  authority,  as  defined  by  the  Member State.
 *
 *
 *
 * This is the proccess of Calculating night time in this program:
 *
 * 1) A flight from A to B follows the Great Circle Track along these two points
 *    at an average cruising height of 11km. (~FL 360)
 *
 * 2) Any time the Elevation of the Sun at the current position is less
 *    than -6 degrees, night conditions are present.
 * 3) The Calculation is performed for every minute of flight time.
 *
 * In general, input and output for most functions is decimal degrees, like coordinates
 * are stowed in the airports table. Calculations are normally done using
 * Radians.
 */

/*!
 * \brief radToDeg Converts radians to degrees
 * \param rad
 * \return degrees
 */
double Calc::radToDeg(double rad)
{
    double deg = rad * (180 / M_PI);
    return deg;
}

/*!
 * \brief degToRad Converts degrees to radians
 * \param deg
 * \return radians
 */
double Calc::degToRad(double deg)
{
    double rad = deg * (M_PI / 180);
    return rad;
}

/*!
 * \brief radToNauticalMiles Convert Radians to nautical miles
 * \param rad
 * \return nautical miles
 */
double Calc::radToNauticalMiles(double rad)
{
    double nm = rad * 3440.06479482;
    return nm;
}

/*!
 * \brief greatCircleDistance Calculates Great Circle distance between two coordinates, return in Radians.
 * \param lat1 Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon1 Location Longitude in degrees -180:180 W(-) E(+)
 * \param lat2 Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon2 Location Longitude in degrees -180:180 W(-) E(+)
 * \return
 */
double Calc::greatCircleDistance(double lat1, double lon1, double lat2, double lon2)
{
    // Converting Latitude and Longitude to Radians
    lat1 = degToRad(lat1);
    lon1 = degToRad(lon1);
    lat2 = degToRad(lat2);
    lon2 = degToRad(lon2);

    // Haversine Formula
    double deltalon = lon2 - lon1;
    double deltalat = lat2 - lat1;

    double result = pow(sin(deltalat / 2), 2) +
                    cos(lat1) * cos(lat2) * pow(sin(deltalon / 2), 2);
    result = 2 * asin(sqrt(result));
    return result;
}

/*!
 * \brief Calc::greatCircleDistanceBetweenAirports Calculates Great
 * Circle distance between two coordinates, return in nautical miles.
 * \param dept ICAO 4-letter Airport Identifier
 * \param dest ICAO 4-letter Airport Identifier
 * \return Nautical Miles From Departure to Destination
 */
double Calc::greatCircleDistanceBetweenAirports(QString dept, QString dest)
{
    //db::multiSelect("airports", columns, "EDDF", "icao", db::exactMatch);
    /*if(dbAirport::retreiveIcaoCoordinates(dept).isEmpty() || dbAirport::retreiveIcaoCoordinates(dest).isEmpty()){
        qWarning() << "greatCircleDistance - invalid input. aborting.";
        return 0;
    }*/

    QVector<QString> columns = {"lat", "long"};
    QVector<QString> deptCoordinates = Db::multiSelect(columns, "airports", "icao", dept,
                                                       Db::exactMatch);
    QVector<QString> destCoordinates = Db::multiSelect(columns, "airports", "icao", dest,
                                                       Db::exactMatch);

    if (deptCoordinates.isEmpty() || destCoordinates.isEmpty()
       ) {
        qDebug() << "greatCircleDistance - invalid input. aborting.";
        return 0;
    }

    double lat1 = degToRad(deptCoordinates[0].toDouble());
    double lon1 = degToRad(deptCoordinates[1].toDouble());
    double lat2 = degToRad(destCoordinates[0].toDouble());
    double lon2 = degToRad(destCoordinates[1].toDouble());

    // Haversine Formula
    double deltalon = lon2 - lon1;
    double deltalat = lat2 - lat1;

    double result = pow(sin(deltalat / 2), 2) +
                    cos(lat1) * cos(lat2) * pow(sin(deltalon / 2), 2);
    result = 2 * asin(sqrt(result));
    return radToNauticalMiles(result);
}

/*!
 * \brief  Calculates a list of points (lat,lon) along the Great Circle between two points.
 * The points are spaced equally, one minute of block time apart.
 * \param lat1 Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon1 Location Longitude in degrees -180:180 W(-) E(+)
 * \param lat2 Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon2 Location Longitude in degrees -180:180 W(-) E(+)
 * \param tblk Total Blocktime in minutes
 * \return coordinates {lat,lon} along the Great Circle Track
 */
QVector<QVector<double>> Calc::intermediatePointsOnGreatCircle(double lat1, double lon1,
                                                               double lat2, double lon2, int tblk)
{
    double d = greatCircleDistance(lat1, lon1, lat2, lon2); //Calculate distance (radians)
    // Converting Latitude and Longitude to Radians
    lat1 = degToRad(lat1);
    lon1 = degToRad(lon1);
    lat2 = degToRad(lat2);
    lon2 = degToRad(lon2);

    //loop for creating one minute steps along the Great Circle
    // 0 is departure point, 1 is end point
    QVector<QVector<double>> coordinates;
    double fraction = 1.0 / tblk;
    for (int i = 0; i <= tblk; i++) {
        // Calculating intermediate point for fraction of distance
        double A = sin((1 - fraction * i) * d) / sin(d);
        double B = sin(fraction * i * d) / sin(d);
        double x = A * cos(lat1) * cos(lon1) + B * cos(lat2) * cos(lon2);
        double y = A * cos(lat1) * sin(lon1) + B * cos(lat2) * sin(lon2);
        double z = A * sin(lat1) + B * sin(lat2);
        double lat = atan2(z, sqrt( pow(x, 2) + pow(y, 2) ));
        double lon = atan2(y, x);

        QVector<double> coordinate = {lat, lon};
        coordinates.append(coordinate);
    }
    return coordinates;
}

/*!
 * \brief Calculates solar elevation angle for a given point in time and latitude/longitude coordinates
 *
 * It is based on the formulas found here: http://stjarnhimlen.se/comp/tutorial.html#5
 *
 * Credit also goes to Darin C. Koblick for his matlab implementation of various of these
 * formulas and to Kevin Godden for porting it to C++.
 *
 * Darin C. Koblock: https://www.mathworks.com/matlabcentral/profile/authors/1284781
 * Kevin Godden: https://www.ridgesolutions.ie/index.php/about-us/
 *
 * \param utc_time_point - QDateTime (UTC) for which the elevation is Calculated
 * \param lat - Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon - Location Longitude in degrees -180:180 W(-) E(+)
 * \return elevation - double of solar elevation in degrees.
 */
double Calc::solarElevation(QDateTime utc_time_point, double lat, double lon)
{
    double Alt =
        11; // I am taking 11 kilometers as an average cruising height for a commercial passenger airplane.
    // convert current DateTime Object to a J2000 value used in the Calculation
    double d = utc_time_point.date().toJulianDay() - 2451544 + utc_time_point.time().hour() / 24.0 +
               utc_time_point.time().minute() / 1440.0;

    // Orbital Elements (in degress)
    double w = 282.9404 + 4.70935e-5 * d; // (longitude of perihelion)
    double e = 0.016709 - 1.151e-9 * d; // (eccentricity)
    double M = fmod(356.0470 + 0.9856002585 * d,
                    360.0); // (mean anomaly, needs to be between 0 and 360 degrees)
    double oblecl = 23.4393 - 3.563e-7 * d; // (Sun's obliquity of the ecliptic)
    double L = w + M; // (Sun's mean longitude)
    // auxiliary angle
    double  E = M + (180 / M_PI) * e * sin(M * (M_PI / 180)) * (1 + e * cos(M * (M_PI / 180)));
    // The Sun's rectangular coordinates in the plane of the ecliptic
    double x = cos(E * (M_PI / 180)) - e;
    double y = sin(E * (M_PI / 180)) * sqrt(1 - pow(e, 2));
    // find the distance and true anomaly
    double r = sqrt(pow(x, 2) + pow(y, 2));
    double v = atan2(y, x) * (180 / M_PI);
    // find the longitude of the sun
    double solarlongitude = v + w;
    // compute the ecliptic rectangular coordinates
    double xeclip = r * cos(solarlongitude * (M_PI / 180));
    double yeclip = r * sin(solarlongitude * (M_PI / 180));
    double zeclip = 0.0;
    //rotate these coordinates to equitorial rectangular coordinates
    double xequat = xeclip;
    double yequat = yeclip * cos(oblecl * (M_PI / 180)) + zeclip * sin(oblecl * (M_PI / 180));
    double zequat = yeclip * sin(23.4406 * (M_PI / 180)) + zeclip * cos(oblecl * (M_PI / 180));
    // convert equatorial rectangular coordinates to RA and Decl:
    r = sqrt(pow(xequat, 2) + pow(yequat, 2) + pow(zequat,
                                                   2)) - (Alt / 149598000); //roll up the altitude correction
    double RA = atan2(yequat, xequat) * (180 / M_PI);
    double delta = asin(zequat / r) * (180 / M_PI);

    // GET UTH time
    double UTH = utc_time_point.time().hour() + utc_time_point.time().minute() / 60.0 +
                 utc_time_point.time().second() / 3600.0;
    // Calculate local siderial time
    double GMST0 = fmod(L + 180, 360.0) / 15;
    double SIDTIME = GMST0 + UTH + lon / 15;
    // Replace RA with hour angle HA
    double HA = (SIDTIME * 15 - RA);
    // convert to rectangular coordinate system
    x = cos(HA * (M_PI / 180)) * cos(delta * (M_PI / 180));
    y = sin(HA * (M_PI / 180)) * cos(delta * (M_PI / 180));
    double z = sin(delta * (M_PI / 180));
    // rotate this along an axis going east - west.
    double zhor = x * sin((90 - lat) * (M_PI / 180)) + z * cos((90 - lat) * (M_PI / 180));

    // Find the Elevation
    double elevation = asin(zhor) * (180 / M_PI);
    return elevation;
}

/*!
 * \brief Calculates which portion of a flight was conducted in night conditions.
 * \param dept - ICAO 4-letter code of Departure Airport
 * \param dest - ICAO 4-letter Code of Destination Airport
 * \param departureTime - QDateTime of Departure (UTC)
 * \param tblk - Total block time in minutes
 * \param nightAngle - the solar elevation angle where night conditons exist.
 * Default -6 (end of civil evening twilight)
 * \return Total number of minutes under night flying conditions
 */
int Calc::calculateNightTime(QString dept, QString dest, QDateTime departureTime, int tblk, int nightAngle)
{
    QVector<QString> columns = {"lat", "long"};
    QVector<QString> deptCoordinates = Db::multiSelect(columns, "airports", "icao", dept,
                                                       Db::exactMatch);
    QVector<QString> destCoordinates = Db::multiSelect(columns, "airports", "icao", dest,
                                                       Db::exactMatch);

    if (deptCoordinates.isEmpty() || destCoordinates.isEmpty()
       ) {
        qDebug() << "Calc::CalculateNightTime - invalid input. aborting.";
        return 0;
    }

    double deptLat = degToRad(deptCoordinates[0].toDouble());
    double deptLon = degToRad(deptCoordinates[1].toDouble());
    double destLat = degToRad(destCoordinates[0].toDouble());
    double destLon = degToRad(destCoordinates[1].toDouble());

    qDebug() << "Calc::CalculateNightTime deptLat = " << deptLat;
    qDebug() << "Calc::CalculateNightTime deptLon = " << deptLon;
    qDebug() << "Calc::CalculateNightTime destLat = " << destLat;
    qDebug() << "Calc::CalculateNightTime destLon = " << destLon;

    QVector<QVector<double>> route = intermediatePointsOnGreatCircle(deptLat, deptLon, destLat, destLon,
                                                                     tblk);

    int nightTime = 0;
    for (int i = 0; i < tblk ; i++) {
        if (solarElevation(departureTime.addSecs(60 * i), radToDeg(route[i][0]),
                           radToDeg(route[i][1])) < nightAngle) {
            nightTime ++;
        }
    }
    qDebug() << "Calc::CalculateNightTime result for angle: "<< nightAngle
             << " :" << nightTime << " minutes night flying time.";
    return nightTime;
}

/*!
 * \brief Calc::formatTimeInput verifies user input and formats to hh:mm
 * if the output is not a valid time, an empty string is returned. Accepts
 * input as hh:mm, h:mm, hhmm or hmm.
 * \param userinput from a QLineEdit
 * \return formatted QString "hh:mm" or Empty String
 */
QString Calc::formatTimeInput(QString userinput)
{
    QString output; //
    QTime temptime; //empty time object is invalid by default

    bool containsSeperator = userinput.contains(":");
    if (userinput.length() == 4 && !containsSeperator) {
        temptime = QTime::fromString(userinput, "hhmm");
    } else if (userinput.length() == 3 && !containsSeperator) {
        if (userinput.toInt() < 240) { //Qtime is invalid if time is between 000 and 240 for this case
            QString tempstring = userinput.prepend("0");
            temptime = QTime::fromString(tempstring, "hhmm");
        } else {
            temptime = QTime::fromString(userinput, "Hmm");
        }
    } else if (userinput.length() == 4 && containsSeperator) {
        temptime = QTime::fromString(userinput, "h:mm");
    } else if (userinput.length() == 5 && containsSeperator) {
        temptime = QTime::fromString(userinput, "hh:mm");
    }

    output = temptime.toString("hh:mm");
    if (output.isEmpty()) {
        qDebug() << "Time input is invalid.";
    }
    return output;
}

/*!
 * \brief Calc::updateAutoTimes When the details of an aircraft are changed,
 * this function recalculates deductable times for this aircraft and updates
 * the database accordingly.
 * \param acft An aircraft object.
 * \return
 */
void Calc::updateAutoTimes(int acft_id)
{
    //find all flights for aircraft
    auto flightList = Db::multiSelect(QVector<QString>{"id"},"flights","acft",
                                      QString::number(acft_id),Db::exactMatch);
    auto acft = Aircraft(acft_id);
    for (const auto& item : flightList) {
        auto flt = Flight(item.toInt());

        if(acft.data.value("singlepilot") == "1" && acft.data.value("singleengine") == "1") {
            DEB("SPSE");
            flt.data.insert("tSPSE",flt.data.value("tblk"));
            flt.data.insert("tSPME","");
            flt.data.insert("tMP","");
        } else if ((acft.data.value("singlepilot") == "1" && acft.data.value("multiengine") == "1")) {
            DEB("SPME");
            flt.data.insert("tSPME",flt.data.value("tblk"));
            flt.data.insert("tSPSE","");
            flt.data.insert("tMP","");
        } else if ((acft.data.value("multipilot") == "1")) {
            DEB("MPME");
            flt.data.insert("tMP",flt.data.value("tblk"));
            flt.data.insert("tSPSE","");
            flt.data.insert("tSPME","");
        }
        flt.commit();
    }
}
/*!
 * \brief Calc::updateNightTimes updates the night times in the database
 */
void Calc::updateNightTimes()
{
    const int nightAngle = Settings::read("flightlogging/nightangle").toInt();
    const QVector<QString> columns = {"id"};
    auto flights = Db::multiSelect(columns,"flights");
    for (const auto& item : flights) {
        auto flt = new Flight(item.toInt());
        auto dateTime = QDateTime(QDate::fromString(flt->data.value("doft"),Qt::ISODate),
                                  QTime().addSecs(flt->data.value("tofb").toInt() * 60),
                                  Qt::UTC);
        flt->data.insert("tNIGHT", QString::number(
                             calculateNightTime(flt->data.value("dept"),
                             flt->data.value("dest"),
                             dateTime,
                             flt->data.value("tblk").toInt(),
                             nightAngle)));
        flt->commit();
    }
}
