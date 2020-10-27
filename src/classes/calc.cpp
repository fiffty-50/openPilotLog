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
//#include "dbman.cpp"
/*!
 * \brief calc::blocktime Calculates Block Time for a given departure and arrival time
 * \param tofb QTime Time Off Blocks
 * \param tonb QTime Time On Blocks
 * \return Block Time in minutes
 */
QTime calc::blocktime(QTime tofb, QTime tonb)
{
    if(tonb > tofb)// landing same day
    {
        QTime blocktimeout(0,0); // initialise return value at midnight
        int blockseconds = tofb.secsTo(tonb); // returns seconds between 2 time objects
        blocktimeout = blocktimeout.addSecs(blockseconds);
        return blocktimeout;

    } else // landing next day
    {
        QTime midnight(0,0);
        QTime blocktimeout(0,0); // initialise return value at midnight
        int blockseconds = tofb.secsTo(midnight); // returns seconds passed until midnight
        blocktimeout = blocktimeout.addSecs(blockseconds);
        blockseconds = midnight.secsTo(tonb); // returns seconds passed after midnight
        blocktimeout = blocktimeout.addSecs(blockseconds);
        return blocktimeout;
    }
}


/*!
 * \brief calc::minutes_to_string Converts database time to String Time
 * \param blockminutes int from database
 * \return String hh:mm
 */
QString calc::minutes_to_string(QString blockminutes)
{
    int minutes = blockminutes.toInt();
    QString hour = QString::number(minutes/60);
    if (hour.size() < 2) {hour.prepend("0");}
    QString minute = QString::number(minutes % 60);
    if (minute.size() < 2) {minute.prepend("0");}
    QString blocktime = hour + ":" + minute;
    return blocktime;
};

/*!
 * \brief calc::time_to_minutes converts QTime to int minutes
 * \param time QTime
 * \return int time as number of minutes
 */
int calc::time_to_minutes(QTime time)
{
    QString timestring = time.toString("hh:mm");
    int minutes = (timestring.left(2).toInt()) * 60;
    minutes += timestring.right(2).toInt();
    return minutes;
}

/*!
 * \brief calc::string_to_minutes Converts String Time to String Number of Minutes
 * \param timestring "hh:mm"
 * \return String number of minutes
 */
int calc::string_to_minutes(QString timestring)
{
    int minutes = (timestring.left(2).toInt()) * 60;
    minutes += timestring.right(2).toInt();
    timestring = QString::number(minutes);
    return minutes;
}

/*!
 * The purpose of the following functions is to provide functionality enabling the calculation of
 * night flying time. EASA defines night as follows:
 *
 * ‘Night’  means  the  period  between  the  end  of  evening  civil  twilight  and  the  beginning  of
 * morning  civil  twilight  or  such  other  period  between  sunset  and  sunrise  as  may  be  prescribed
 * by  the  appropriate  authority,  as  defined  by  the  Member State.
 *
 *
 *
 * This is the proccess of calculating night time in this program:
 *
 * 1) A flight from A to B follows the Great Circle Track along these two points
 *    at an average cruising height of 11km. (~FL 360)
 *
 * 2) Any time the Elevation of the Sun at the current position is less
 *    than -6 degrees, night conditions are present.
 * 3) The calculation is performed for every minute of flight time.
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
double calc::radToDeg(double rad)
{
    double deg = rad * (180 / M_PI);
    return deg;
}

/*!
 * \brief degToRad Converts degrees to radians
 * \param deg
 * \return radians
 */
double calc::degToRad(double deg)
{
    double rad = deg * (M_PI / 180);
    return rad;
}

/*!
 * \brief radToNauticalMiles Convert Radians to nautical miles
 * \param rad
 * \return nautical miles
 */
double calc::radToNauticalMiles(double rad)
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
double calc::greatCircleDistance(double lat1, double lon1, double lat2, double lon2)
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
 * \brief calc::greatCircleDistanceBetweenAirports Calculates Great
 * Circle distance between two coordinates, return in nautical miles.
 * \param dept ICAO 4-letter Airport Identifier
 * \param dest ICAO 4-letter Airport Identifier
 * \return Nautical Miles From Departure to Destination
 */
double calc::greatCircleDistanceBetweenAirports(QString dept, QString dest)
{
    //db::multiSelect("airports", columns, "EDDF", "icao", sql::exactMatch);
    /*if(dbAirport::retreiveIcaoCoordinates(dept).isEmpty() || dbAirport::retreiveIcaoCoordinates(dest).isEmpty()){
        qWarning() << "greatCircleDistance - invalid input. aborting.";
        return 0;
    }*/

    QVector<QString> columns = {"lat", "long"};
    QVector<QString> deptCoordinates = db::multiSelect(columns,"airports", "icao", dept, sql::exactMatch);
    QVector<QString> destCoordinates = db::multiSelect(columns,"airports", "icao", dest, sql::exactMatch);

    if(deptCoordinates.isEmpty() || destCoordinates.isEmpty()
       )
    {
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
QVector<QVector<double>> calc::intermediatePointsOnGreatCircle(double lat1, double lon1, double lat2, double lon2, int tblk)
{
    double d = greatCircleDistance(lat1, lon1, lat2, lon2); //calculate distance (radians)
    // Converting Latitude and Longitude to Radians
    lat1 = degToRad(lat1);
    lon1 = degToRad(lon1);
    lat2 = degToRad(lat2);
    lon2 = degToRad(lon2);

    //loop for creating one minute steps along the Great Circle
    // 0 is departure point, 1 is end point
    QVector<QVector<double>> coordinates;
    double fraction = 1.0/tblk;
    for(int i = 0; i <= tblk; i++) {
        // Calculating intermediate point for fraction of distance
        double A=sin((1-fraction * i) * d)/sin(d);
        double B=sin(fraction * i * d)/sin(d);
        double x = A*cos(lat1) * cos(lon1) + B * cos(lat2) * cos(lon2);
        double y = A*cos(lat1) * sin(lon1) + B * cos(lat2) * sin(lon2);
        double z = A*sin(lat1) + B * sin(lat2);
        double lat = atan2(z, sqrt( pow(x, 2) + pow(y, 2) ));
        double lon = atan2(y, x);

        QVector<double> coordinate = {lat,lon};
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
 * \param utc_time_point - QDateTime (UTC) for which the elevation is calculated
 * \param lat - Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon - Location Longitude in degrees -180:180 W(-) E(+)
 * \return elevation - double of solar elevation in degrees.
 */
double calc::solarElevation(QDateTime utc_time_point, double lat, double lon)
{
    double Alt = 11; // I am taking 11 kilometers as an average cruising height for a commercial passenger airplane.
    // convert current DateTime Object to a J2000 value used in the calculation
    double d = utc_time_point.date().toJulianDay() - 2451544 + utc_time_point.time().hour()/24.0 + utc_time_point.time().minute()/1440.0;

    // Orbital Elements (in degress)
    double w = 282.9404 + 4.70935e-5 * d; // (longitude of perihelion)
    double e = 0.016709 - 1.151e-9 * d; // (eccentricity)
    double M = fmod(356.0470 + 0.9856002585 * d, 360.0); // (mean anomaly, needs to be between 0 and 360 degrees)
    double oblecl = 23.4393 - 3.563e-7*d; // (Sun's obliquity of the ecliptic)
    double L = w + M; // (Sun's mean longitude)
    // auxiliary angle
    double  E = M + (180 / M_PI)*e*sin(M*(M_PI / 180))*(1 + e*cos(M*(M_PI / 180)));
    // The Sun's rectangular coordinates in the plane of the ecliptic
    double x = cos(E*(M_PI / 180)) - e;
    double y = sin(E*(M_PI / 180))*sqrt(1 - pow(e, 2));
    // find the distance and true anomaly
    double r = sqrt(pow(x,2) + pow(y,2));
    double v = atan2(y, x)*(180 / M_PI);
    // find the longitude of the sun
    double solarlongitude = v + w;
    // compute the ecliptic rectangular coordinates
    double xeclip = r*cos(solarlongitude*(M_PI / 180));
    double yeclip = r*sin(solarlongitude*(M_PI / 180));
    double zeclip = 0.0;
    //rotate these coordinates to equitorial rectangular coordinates
    double xequat = xeclip;
    double yequat = yeclip*cos(oblecl*(M_PI / 180)) + zeclip * sin(oblecl*(M_PI / 180));
    double zequat = yeclip*sin(23.4406*(M_PI / 180)) + zeclip * cos(oblecl*(M_PI / 180));
    // convert equatorial rectangular coordinates to RA and Decl:
    r = sqrt(pow(xequat, 2) + pow(yequat, 2) + pow(zequat, 2)) - (Alt / 149598000); //roll up the altitude correction
    double RA = atan2(yequat, xequat)*(180 / M_PI);
    double delta = asin(zequat / r)*(180 / M_PI);

    // GET UTH time
    double UTH = utc_time_point.time().hour() + utc_time_point.time().minute()/60.0 + utc_time_point.time().second()/3600.0;
    // Calculate local siderial time
    double GMST0 = fmod(L + 180, 360.0) / 15;
    double SIDTIME = GMST0 + UTH + lon / 15;
    // Replace RA with hour angle HA
    double HA = (SIDTIME*15 - RA);
    // convert to rectangular coordinate system
    x = cos(HA*(M_PI / 180))*cos(delta*(M_PI / 180));
    y = sin(HA*(M_PI / 180))*cos(delta*(M_PI / 180));
    double z = sin(delta*(M_PI / 180));
    // rotate this along an axis going east - west.
    double zhor = x*sin((90 - lat)*(M_PI / 180)) + z*cos((90 - lat)*(M_PI / 180));

    // Find the Elevation
    double elevation = asin(zhor)*(180 / M_PI);
    return elevation;
}

/*!
 * \brief Calculates which portion of a flight was conducted in night conditions.
 * \param dept - ICAO 4-letter code of Departure Airport
 * \param dest - ICAO 4-letter Code of Destination Airport
 * \param departureTime - QDateTime of Departure (UTC)
 * \param tblk - Total block time in minutes
 * \return Total number of minutes under night flying conditions
 */
int calc::calculateNightTime(QString dept, QString dest, QDateTime departureTime, int tblk)
{
    QVector<QString> columns = {"lat", "long"};
    QVector<QString> deptCoordinates = db::multiSelect(columns,"airports", "icao", dept, sql::exactMatch);
    QVector<QString> destCoordinates = db::multiSelect(columns,"airports", "icao", dest, sql::exactMatch);

    if(deptCoordinates.isEmpty() || destCoordinates.isEmpty()
       )
    {
        qDebug() << "calc::calculateNightTime - invalid input. aborting.";
        return 0;
    }

    double deptLat = degToRad(deptCoordinates[0].toDouble());
    double deptLon = degToRad(deptCoordinates[1].toDouble());
    double destLat = degToRad(destCoordinates[0].toDouble());
    double destLon = degToRad(destCoordinates[1].toDouble());

    qDebug() << "calc::calculateNightTime deptLat = " << deptLat;
    qDebug() << "calc::calculateNightTime deptLon = " << deptLon;
    qDebug() << "calc::calculateNightTime destLat = " << destLat;
    qDebug() << "calc::calculateNightTime destLon = " << destLon;

    QVector<QVector<double>> route = intermediatePointsOnGreatCircle(deptLat, deptLon, destLat, destLon, tblk);

    int nightTime = 0;
    for(int i = 0; i < tblk ; i++) {
        if(solarElevation(departureTime.addSecs(60*i),radToDeg(route[i][0]),radToDeg(route[i][1])) < -0.6) {
            nightTime ++;
        }
    }
    qDebug() << "calc::calculateNightTime result: " << nightTime << " minutes night flying time.";
    return nightTime;
}

/*!
 * \brief calc::formatTimeInput verifies user input and formats to hh:mm
 * if the output is not a valid time, an empty string is returned. Accepts
 * input as hh:mm, h:mm, hhmm or hmm.
 * \param userinput from a QLineEdit
 * \return formatted QString "hh:mm" or Empty String
 */
QString calc::formatTimeInput(QString userinput)
{
    QString output; //
    QTime temptime; //empty time object is invalid by default

    bool containsSeperator = userinput.contains(":");
        if(userinput.length() == 4 && !containsSeperator)
        {
            temptime = QTime::fromString(userinput,"hhmm");
        }else if(userinput.length() == 3 && !containsSeperator)
        {
            if(userinput.toInt() < 240) //Qtime is invalid if time is between 000 and 240 for this case
            {
                QString tempstring = userinput.prepend("0");
                temptime = QTime::fromString(tempstring,"hhmm");
            }else
            {
                temptime = QTime::fromString(userinput,"Hmm");
            }
        }else if(userinput.length() == 4 && containsSeperator)
        {
            temptime = QTime::fromString(userinput,"h:mm");
        }else if(userinput.length() == 5 && containsSeperator)
        {
            temptime = QTime::fromString(userinput,"hh:mm");
        }

        output = temptime.toString("hh:mm");
        if(output.isEmpty())
        {
            /*QMessageBox timeformat(this);
            timeformat.setText("Please enter a valid time. Any of these formats is valid:\n845 0845 8:45 08:45");
            timeformat.exec();*/
            qDebug() << "Time input is invalid.";
        }
        return output;
}