#include "acalc.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/classes/asettings.h"
#include "src/oplconstants.h"

using namespace ACalc;

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

    bool contains_seperator = user_input.contains(':');
    if (user_input.length() == 4 && !contains_seperator) {
        temp_time = QTime::fromString(user_input, QStringLiteral("hhmm"));
    } else if (user_input.length() == 3 && !contains_seperator) {
        if (user_input.toInt() < 240) { //Qtime is invalid if time is between 000 and 240 for this case
            QString tempstring = user_input.prepend(QStringLiteral("0"));
            temp_time = QTime::fromString(tempstring, QStringLiteral("hhmm"));
        } else {
            temp_time = QTime::fromString(user_input, QStringLiteral("Hmm"));
        }
    } else if (user_input.length() == 4 && contains_seperator) {
        temp_time = QTime::fromString(user_input, QStringLiteral("h:mm"));
    } else if (user_input.length() == 5 && contains_seperator) {
        temp_time = QTime::fromString(user_input, QStringLiteral("hh:mm"));
    }

    output = temp_time.toString(QStringLiteral("hh:mm"));
    if (output.isEmpty()) {
        DEB << "Time input is invalid.";
    }
    return output;
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


double ACalc::greatCircleDistanceBetweenAirports(const QString &dept, const QString &dest)
{
    auto statement = "SELECT lat, long FROM airports WHERE icao = '"
            + dept + "' OR icao = '" + dest + "'";
    auto lat_lon = aDB->customQuery(statement, 2);

    if (lat_lon.length() != 4) {
        DEB << "Invalid input. Aborting.";
        return 0;
    }

    double dept_lat = degToRad(lat_lon[0].toDouble());
    double dept_lon = degToRad(lat_lon[1].toDouble());
    double dest_lat = degToRad(lat_lon[2].toDouble());
    double dest_lon = degToRad(lat_lon[3].toDouble());

    // Haversine Formula
    double delta_lon = dest_lon - dept_lon;
    double delta_lat = dest_lat - dept_lat;

    double result = pow(sin(delta_lat / 2), 2) +
                    cos(dept_lat) * cos(dest_lat) * pow(sin(delta_lon / 2), 2);
    result = 2 * asin(sqrt(result));
    return radToNauticalMiles(result);
}


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


int ACalc::calculateNightTime(const QString &dept, const QString &dest, QDateTime departureTime, int tblk, int nightAngle)
{

    auto statement = "SELECT lat, long FROM airports WHERE icao = '" + dept + "' OR icao = '" + dest + "'";
    auto lat_lon = aDB->customQuery(statement, 2);

    if (lat_lon.length() != 4) {
        DEB << "Invalid input. Aborting.";
        return 0;
    }

    double dept_lat = degToRad(lat_lon[0].toDouble());
    double dept_lon = degToRad(lat_lon[1].toDouble());
    double dest_lat = degToRad(lat_lon[2].toDouble());
    double dest_lon = degToRad(lat_lon[3].toDouble());

    QVector<QVector<double>> route = intermediatePointsOnGreatCircle(dept_lat, dept_lon,
                                                                     dest_lat, dest_lon,
                                                                     tblk);
    int night_time = 0;
    for (int i = 0; i < tblk ; i++) {
        if (solarElevation(departureTime.addSecs(60 * i), radToDeg(route[i][0]),
                           radToDeg(route[i][1])) < nightAngle) {
            night_time ++;
        }
    }
    return night_time;
}

bool ACalc::isNight(const QString &icao, QDateTime event_time, int night_angle)
{
    auto statement = "SELECT lat, long FROM airports WHERE icao = '" + icao + "'";
    auto lat_lon = aDB->customQuery(statement, 2);

    if (lat_lon.length() != 2) {
        DEB << "Invalid input. Aborting.";
        return 0;
    }

    double lat = degToRad(lat_lon[0].toDouble());
    double lon = degToRad(lat_lon[1].toDouble());

    if(solarElevation(event_time, lat, lon) < night_angle){
        return true;
    } else {
        return false;
    }
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
    const QString statement = QStringLiteral("SELECT flight_id FROM flights WHERE acft = ") + QString::number(acft_id);
    auto flight_list = aDB->customQuery(statement, 1);
    if (flight_list.isEmpty()) {
        DEB << "No flights for this tail found.";
        return;
    }
    DEB << "Updating " << flight_list.length() << " flights with this aircraft.";

    auto acft = aDB->getTailEntry(acft_id);
    auto acft_data = acft.getData();
    for (const auto& item : flight_list) {
        auto flight = aDB->getFlightEntry(item.toInt());
        auto flight_data = flight.getData();

        if(acft_data.value(opl::db::TAILS_MULTIPILOT).toInt() == 0
                && acft_data.value(opl::db::TAILS_MULTIENGINE) == 0) {
            DEB << "SPSE";
            flight_data.insert(opl::db::FLIGHTS_TSPSE, flight_data.value(opl::db::FLIGHTS_TBLK));
            flight_data.insert(opl::db::FLIGHTS_TSPME, QStringLiteral(""));
            flight_data.insert(opl::db::FLIGHTS_TMP, QStringLiteral(""));
        } else if ((acft_data.value(opl::db::TAILS_MULTIPILOT) == 0
                    && acft.getData().value(opl::db::TAILS_MULTIENGINE) == 1)) {
            DEB << "SPME";
            flight_data.insert(opl::db::FLIGHTS_TSPME, flight_data.value(opl::db::FLIGHTS_TBLK));
            flight_data.insert(opl::db::FLIGHTS_TSPSE, QStringLiteral(""));
            flight_data.insert(opl::db::FLIGHTS_TMP, QStringLiteral(""));
        } else if ((acft_data.value(opl::db::TAILS_MULTIPILOT) == 1)) {
            DEB << "MPME";
            flight_data.insert(opl::db::FLIGHTS_TMP, flight_data.value(opl::db::FLIGHTS_TBLK));
            flight_data.insert(opl::db::FLIGHTS_TSPSE, QStringLiteral(""));
            flight_data.insert(opl::db::FLIGHTS_TSPME, QStringLiteral(""));
        }
        flight.setData(flight_data);
        aDB->commit(flight);
    }
}
/*!
 * \brief ACalc::updateNightTimes updates the night times in the database,
 * used when changing night angle setting for example
 */
void ACalc::updateNightTimes()
{
    const int night_angle = ASettings::read(ASettings::FlightLogging::NightAngle).toInt();

    //find all flights for aircraft
    auto statement = "SELECT ROWID FROM flights";
    auto flight_list = aDB->customQuery(statement, 1);

    if (flight_list.isEmpty()) {
        DEB << "No flights found.";
        return;
    }
    DEB << "Updating " << flight_list.length() << " flights in the database.";

    for (const auto& item : flight_list) {

        auto flt = aDB->getFlightEntry(item.toInt());
        auto data = flt.getData();
        auto dateTime = QDateTime(QDate::fromString(data.value(opl::db::FLIGHTS_DOFT).toString(), Qt::ISODate),
                                  QTime().addSecs(data.value(opl::db::FLIGHTS_TOFB).toInt() * 60),
                                  Qt::UTC);
        data.insert(opl::db::FLIGHTS_TNIGHT,
                    calculateNightTime(data.value(opl::db::FLIGHTS_DEPT).toString(),
                                       data.value(opl::db::FLIGHTS_DEST).toString(),
                                       dateTime,
                                       data.value(opl::db::FLIGHTS_TBLK).toInt(),
                                       night_angle));
        flt.setData(data);
        aDB->commit(flt);
    }
}

