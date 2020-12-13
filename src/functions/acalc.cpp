#include "acalc.h"
#include "debug.h"

using namespace ACalc;

/*!
 * \brief ACalc::blocktime Calculates Block Time for a given departure and arrival time
 * \param tofb QTime Time Off Blocks
 * \param tonb QTime Time On Blocks
 * \return Block Time in minutes
 */
QTime ACalc::blocktime(QTime tofb, QTime tonb)
{
    QTime blocktime_out(0, 0); // initialise return value at midnight

    if (tonb > tofb) { // landing same day
        int blockseconds = tofb.secsTo(tonb);
        blocktime_out = blocktime_out.addSecs(blockseconds);
    } else { // landing next day
        QTime midnight(0, 0);
        int blockseconds = tofb.secsTo(midnight);
        blocktime_out = blocktime_out.addSecs(blockseconds);
        blockseconds = midnight.secsTo(tonb);
        blocktime_out = blocktime_out.addSecs(blockseconds);
    }
    return blocktime_out;
}


/*!
 * \brief ACalc::minutes_to_string Converts database time to String Time
 * \param blockminutes from database
 * \return String hh:mm
 */
QString ACalc::minutesToString(QString blockminutes)
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
 * \brief ACalc::time_to_minutes converts QTime to int minutes
 * \param time QTime
 * \return int time as number of minutes
 */
int ACalc::QTimeToMinutes(QTime time)
{
    QString timestring = time.toString("hh:mm");
    int minutes = (timestring.left(2).toInt()) * 60;
    minutes += timestring.right(2).toInt();
    return minutes;
}

/*!
 * \brief ACalc::string_to_minutes Converts String Time to String Number of Minutes
 * \param timestring "hh:mm"
 * \return String number of minutes
 */
int ACalc::stringToMinutes(QString timestring)
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
double ACalc::radToDeg(double rad)
{
    double deg = rad * (180 / M_PI);
    return deg;
}

/*!
 * \brief degToRad Converts degrees to radians
 * \param deg
 * \return radians
 */
double ACalc::degToRad(double deg)
{
    double rad = deg * (M_PI / 180);
    return rad;
}

/*!
 * \brief radToNauticalMiles Convert Radians to nautical miles
 * \param rad
 * \return nautical miles
 */
double ACalc::radToNauticalMiles(double rad)
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
double ACalc::greatCircleDistance(double lat1, double lon1, double lat2, double lon2)
{
    // Converting Latitude and Longitude to Radians
    lat1 = degToRad(lat1);
    lon1 = degToRad(lon1);
    lat2 = degToRad(lat2);
    lon2 = degToRad(lon2);

    // Haversine Formula
    double delta_lon = lon2 - lon1;
    double delta_lat = lat2 - lat1;

    double result = pow(sin(delta_lat / 2), 2) +
                    cos(lat1) * cos(lat2) * pow(sin(delta_lon / 2), 2);
    result = 2 * asin(sqrt(result));
    return result;
}

/*!
 * \brief ACalc::greatCircleDistanceBetweenAirports Calculates Great
 * Circle distance between two coordinates, return in nautical miles.
 * \param dept ICAO 4-letter Airport Identifier
 * \param dest ICAO 4-letter Airport Identifier
 * \return Nautical Miles From Departure to Destination
 */
double ACalc::greatCircleDistanceBetweenAirports(QString dept, QString dest)
{
    QVector<QString> dept_coordinates = Db::multiSelect({"lat", "long"}, "airports", "icao", dept,
                                                       Db::exactMatch);
    QVector<QString> dest_coordinates = Db::multiSelect({"lat", "long"}, "airports", "icao", dest,
                                                       Db::exactMatch);

    if (dept_coordinates.isEmpty() || dest_coordinates.isEmpty()
       ) {
        DEB("invalid input. aborting.");
        return 0;
    }

    double lat1 = degToRad(dept_coordinates[0].toDouble());
    double lon1 = degToRad(dept_coordinates[1].toDouble());
    double lat2 = degToRad(dest_coordinates[0].toDouble());
    double lon2 = degToRad(dest_coordinates[1].toDouble());

    // Haversine Formula
    double delta_lon = lon2 - lon1;
    double delta_lat = lat2 - lat1;

    double result = pow(sin(delta_lat / 2), 2) +
                    cos(lat1) * cos(lat2) * pow(sin(delta_lon / 2), 2);
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
QVector<QVector<double>> ACalc::intermediatePointsOnGreatCircle(double lat1, double lon1,
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
double ACalc::solarElevation(QDateTime utc_time_point, double lat, double lon)
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
    double solar_longitude = v + w;
    // compute the ecliptic rectangular coordinates
    double x_eclip = r * cos(solar_longitude * (M_PI / 180));
    double y_eclip = r * sin(solar_longitude * (M_PI / 180));
    double z_eclip = 0.0;
    //rotate these coordinates to equitorial rectangular coordinates
    double x_equat = x_eclip;
    double y_equat = y_eclip * cos(oblecl * (M_PI / 180)) + z_eclip * sin(oblecl * (M_PI / 180));
    double z_equat = y_eclip * sin(23.4406 * (M_PI / 180)) + z_eclip * cos(oblecl * (M_PI / 180));
    // convert equatorial rectangular coordinates to RA and Decl:
    r = sqrt(pow(x_equat, 2) + pow(y_equat, 2) + pow(z_equat,
                                                   2)) - (Alt / 149598000); //roll up the altitude correction
    double RA = atan2(y_equat, x_equat) * (180 / M_PI);
    double delta = asin(z_equat / r) * (180 / M_PI);

    // GET UT in hours time
    double uth = utc_time_point.time().hour() + utc_time_point.time().minute() / 60.0 +
                 utc_time_point.time().second() / 3600.0;
    // Calculate local siderial time
    double gmst0 = fmod(L + 180, 360.0) / 15;
    double sid_time = gmst0 + uth + lon / 15;
    // Replace RA with hour angle HA
    double HA = (sid_time * 15 - RA);
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
int ACalc::calculateNightTime(const QString &dept, const QString &dest, QDateTime departureTime, int tblk, int nightAngle)
{
    QVector<QString> dept_coordinates = Db::multiSelect({"lat", "long"}, "airports", "icao", dept,
                                                       Db::exactMatch);
    QVector<QString> dest_coordinates = Db::multiSelect({"lat", "long"}, "airports", "icao", dest,
                                                       Db::exactMatch);

    if (dept_coordinates.isEmpty() || dest_coordinates.isEmpty()
       ) {
        DEB("invalid input. aborting.");
        return 0;
    }

    double dept_lat = degToRad(dept_coordinates[0].toDouble());
    double dept_lon = degToRad(dept_coordinates[1].toDouble());
    double dest_lat = degToRad(dest_coordinates[0].toDouble());
    double dest_lon = degToRad(dest_coordinates[1].toDouble());
/*
    qDebug() << "ACalc::CalculateNightTime deptLat = " << deptLat;
    qDebug() << "ACalc::CalculateNightTime deptLon = " << deptLon;
    qDebug() << "ACalc::CalculateNightTime destLat = " << destLat;
    qDebug() << "ACalc::CalculateNightTime destLon = " << destLon;
*/
    QVector<QVector<double>> route = intermediatePointsOnGreatCircle(dept_lat, dept_lon, dest_lat, dest_lon,
                                                                     tblk);

    int night_time = 0;
    for (int i = 0; i < tblk ; i++) {
        if (solarElevation(departureTime.addSecs(60 * i), radToDeg(route[i][0]),
                           radToDeg(route[i][1])) < nightAngle) {
            night_time ++;
        }
    }
    //qDebug() << "ACalc::CalculateNightTime result for angle: "<< nightAngle
    //         << " :" << nightTime << " minutes night flying time.";
    return night_time;
}

bool ACalc::isNight(QString icao, QDateTime event_time, int nightAngle)
{
    QVector<QString> coordinates = Db::multiSelect({"lat", "long"}, "airports", "icao", icao,
                                                       Db::exactMatch);
    if (coordinates.isEmpty()) {
        DEB("invalid input. aborting.");
        return false;
    }

    double lat = degToRad(coordinates[0].toDouble());
    double lon = degToRad(coordinates[1].toDouble());

    if(solarElevation(event_time, lat, lon) < nightAngle){
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief ACalc::formatTimeInput verifies user input and formats to hh:mm
 * if the output is not a valid time, an empty string is returned. Accepts
 * input as hh:mm, h:mm, hhmm or hmm.
 * \param userinput from a QLineEdit
 * \return formatted QString "hh:mm" or Empty String
 */
QString ACalc::formatTimeInput(QString user_input)
{
    QString output; //
    QTime temp_time; //empty time object is invalid by default

    bool contains_seperator = user_input.contains(":");
    if (user_input.length() == 4 && !contains_seperator) {
        temp_time = QTime::fromString(user_input, "hhmm");
    } else if (user_input.length() == 3 && !contains_seperator) {
        if (user_input.toInt() < 240) { //Qtime is invalid if time is between 000 and 240 for this case
            QString tempstring = user_input.prepend("0");
            temp_time = QTime::fromString(tempstring, "hhmm");
        } else {
            temp_time = QTime::fromString(user_input, "Hmm");
        }
    } else if (user_input.length() == 4 && contains_seperator) {
        temp_time = QTime::fromString(user_input, "h:mm");
    } else if (user_input.length() == 5 && contains_seperator) {
        temp_time = QTime::fromString(user_input, "hh:mm");
    }

    output = temp_time.toString("hh:mm");
    if (output.isEmpty()) {
        qDebug() << "Time input is invalid.";
    }
    return output;
}

/*!
 * \brief ACalc::updateAutoTimes When the details of an aircraft are changed,
 * this function recalculates deductable times for this aircraft and updates
 * the database accordingly.
 * \param acft An aircraft object.
 * \return
 */
void ACalc::updateAutoTimes(int acft_id)
{
    //find all flights for aircraft
    auto flight_list = Db::multiSelect({"flight_id"},"flights","acft",
                                      QString::number(acft_id),Db::exactMatch);
    auto acft = Aircraft(acft_id);
    for (const auto& item : flight_list) {
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
 * \brief ACalc::updateNightTimes updates the night times in the database
 */
void ACalc::updateNightTimes()
{
    const int nightAngle = Settings::read("flightlogging/nightangle").toInt();
    auto flights = Db::multiSelect({"flight_id"}, "flights");
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

