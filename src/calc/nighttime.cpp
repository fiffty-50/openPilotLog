/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#include "nighttime.h"
#include "src/database/airportgeographicalinfo.h"
#include "src/opl.h"

/*
constexpr double NightTime::greatCircleDistance(double lat1, double lon1, double lat2, double lon2) {
    lat1 = degToRad(lat1);
    lon1 = degToRad(lon1);
    lat2 = degToRad(lat2);
    lon2 = degToRad(lon2);

    double deltaLat = lat2 - lat1;
    double deltaLon = lon2 - lon1;

    double a = pow(sin(deltaLat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(deltaLon / 2), 2);
    double c = 2 * asin(sqrt(a));
    return c; // in radians
}

QVector<QVector<double> > NightTime::intermediatePointsOnGreatCircle(double lat1, double lon1, double lat2, double lon2, int numSteps) {
    double d = greatCircleDistance(lat1, lon1, lat2, lon2);

    lat1 = degToRad(lat1);
    lon1 = degToRad(lon1);
    lat2 = degToRad(lat2);
    lon2 = degToRad(lon2);

    QVector<QVector<double>> coordinates;
    for (int i = 0; i <= numSteps; ++i) {
        double fraction = static_cast<double>(i) / numSteps;
        double A = sin((1 - fraction) * d) / sin(d);
        double B = sin(fraction * d) / sin(d);

        double x = A * cos(lat1) * cos(lon1) + B * cos(lat2) * cos(lon2);
        double y = A * cos(lat1) * sin(lon1) + B * cos(lat2) * sin(lon2);
        double z = A * sin(lat1) + B * sin(lat2);

        double lat = atan2(z, sqrt(x*x + y*y));
        double lon = atan2(y, x);

        coordinates.append({ radToDeg(lat), radToDeg(lon) });
    }

    return coordinates;
}

double NightTime::solarElevation(const QTime &utc_time, double lat, double lon) {
    double Alt = 11.0; // km cruising altitude

           // Convert QTime to fractional day
    double d = utc_time.hour() / 24.0 + utc_time.minute() / 1440.0 + utc_time.second() / 86400.0;

           // Orbital elements
    double w = 282.9404 + 4.70935e-5 * d;
    double e = 0.016709 - 1.151e-9 * d;
    double M = fmod(356.0470 + 0.9856002585 * d, 360.0);
    double oblecl = 23.4393 - 3.563e-7 * d;
    double L = w + M;

    double E = M + (180.0 / M_PI) * e * sin(degToRad(M)) * (1 + e * cos(degToRad(M)));
    double x = cos(degToRad(E)) - e;
    double y = sin(degToRad(E)) * sqrt(1 - e*e);
    double r = sqrt(x*x + y*y);
    double v = atan2(y, x) * 180.0 / M_PI;
    double solarLongitude = v + w;

    double x_eclip = r * cos(degToRad(solarLongitude));
    double y_eclip = r * sin(degToRad(solarLongitude));
    double z_eclip = 0.0;

    double x_equat = x_eclip;
    double y_equat = y_eclip * cos(degToRad(oblecl)) + z_eclip * sin(degToRad(oblecl));
    double z_equat = y_eclip * sin(degToRad(oblecl)) + z_eclip * cos(degToRad(oblecl));

    r = sqrt(x_equat*x_equat + y_equat*y_equat + z_equat*z_equat) - (Alt / 149598000.0);
    double RA = atan2(y_equat, x_equat) * 180.0 / M_PI;
    double delta = asin(z_equat / r) * 180.0 / M_PI;

    double uth = utc_time.hour() + utc_time.minute()/60.0 + utc_time.second()/3600.0;
    double gmst0 = fmod(L + 180, 360.0)/15.0;
    double sid_time = gmst0 + uth + lon/15.0;
    double HA = sid_time * 15.0 - RA;

    double xhor = cos(degToRad(HA)) * cos(degToRad(delta));
    double zhor = xhor * sin(degToRad(90 - lat)) + sin(degToRad(delta)) * cos(degToRad(90 - lat));
    return radToDeg(asin(zhor));
}

QTime NightTime::nightTime(int dept_airport_id, int dest_airport_id, const QTime &departure_time, int flight_time_minutes, double night_angle) {
    double deptLat = airportGeoData->latitude(dept_airport_id);
    double deptLon = airportGeoData->longitude(dept_airport_id);
    double destLat = airportGeoData->latitude(dest_airport_id);
    double destLon = airportGeoData->longitude(dest_airport_id);

    int nightTime = 0;

    if (dept_airport_id == dest_airport_id) {
        // Local flight
        for (int i = 0; i <= flight_time_minutes; ++i) {
            if (solarElevation(departure_time.addSecs(60*i), deptLat, deptLon) < night_angle)
                nightTime++;
        }
    } else {
        QVector<QVector<double>> route = intermediatePointsOnGreatCircle(deptLat, deptLon, destLat, destLon, flight_time_minutes);
        for (int i = 0; i <= flight_time_minutes; ++i) {
            if (solarElevation(departure_time.addSecs(60*i), route[i][0], route[i][1]) < night_angle)
                nightTime++;
        }
    }

    int time_ms = nightTime * 60 * 1000;

    return QTime::fromMSecsSinceStartOfDay(time_ms);
}

bool NightTime::isNight(int airportID, const QTime &time, double nightAngle) {
    double lat = airportGeoData->latitude(airportID);
    double lon = airportGeoData->longitude(airportID);
    return solarElevation(time, lat, lon) < nightAngle;
}


constexpr double NightTime::greatCircleDistance(double lat1, double lon1, double lat2, double lon2) {
    lat1 = deg_to_rad(lat1);
    lon1 = deg_to_rad(lon1);
    lat2 = deg_to_rad(lat2);
    lon2 = deg_to_rad(lon2);

    double delta_lat = lat2 - lat1;
    double delta_lon = lon2 - lon1;

    double a = pow(sin(delta_lat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(delta_lon / 2), 2);
    return 2 * asin(sqrt(a)); // radians
}

std::vector<std::pair<double,double>> NightTime::intermediate_points_on_great_circle(
    double lat1, double lon1, double lat2, double lon2, int num_steps) {

    double d = greatCircleDistance(lat1, lon1, lat2, lon2);
    lat1 = deg_to_rad(lat1);
    lon1 = deg_to_rad(lon1);
    lat2 = deg_to_rad(lat2);
    lon2 = deg_to_rad(lon2);

    std::vector<std::pair<double,double>> route;
    route.reserve(num_steps + 1);

    for (int i = 0; i < num_steps; ++i) {
        double fraction = static_cast<double>(i) / num_steps;
        double A = sin((1 - fraction) * d) / sin(d);
        double B = sin(fraction * d) / sin(d);

        double x = A * cos(lat1) * cos(lon1) + B * cos(lat2) * cos(lon2);
        double y = A * cos(lat1) * sin(lon1) + B * cos(lat2) * sin(lon2);
        double z = A * sin(lat1) + B * sin(lat2);

        double lat = atan2(z, sqrt(x*x + y*y));
        double lon = atan2(y, x);

        route.emplace_back(rad_to_deg(lat), rad_to_deg(lon));
    }
    return route;
}

double NightTime::solarElevation(int time_ms_since_start_of_day, double lat, double lon,
                                 double sin_lat, double cos_lat) {
    double Alt = 11.0; // km cruising altitude

    double fractional_day = static_cast<double>(time_ms_since_start_of_day) / (24.0 * 3600.0 * 1000.0);

           // Orbital elements
    double w = 282.9404 + 4.70935e-5 * fractional_day;
    double e = 0.016709 - 1.151e-9 * fractional_day;
    double M = fmod(356.0470 + 0.9856002585 * fractional_day, 360.0);
    double oblecl = 23.4393 - 3.563e-7 * fractional_day;
    double L = w + M;

    double E = M + (180.0 / M_PI) * e * sin(deg_to_rad(M)) * (1 + e * cos(deg_to_rad(M)));
    double x = cos(deg_to_rad(E)) - e;
    double y = sin(deg_to_rad(E)) * sqrt(1 - e*e);
    double r = sqrt(x*x + y*y);
    double v = atan2(y, x) * 180.0 / M_PI;
    double solar_longitude = v + w;

    double x_eclip = r * cos(deg_to_rad(solar_longitude));
    double y_eclip = r * sin(deg_to_rad(solar_longitude));
    double z_eclip = 0.0;

    double x_equat = x_eclip;
    double y_equat = y_eclip * cos(deg_to_rad(oblecl)) + z_eclip * sin(deg_to_rad(oblecl));
    double z_equat = y_eclip * sin(deg_to_rad(oblecl)) + z_eclip * cos(deg_to_rad(oblecl));

    r = sqrt(x_equat*x_equat + y_equat*y_equat + z_equat*z_equat) - (Alt / 149598000.0);
    double RA = atan2(y_equat, x_equat) * 180.0 / M_PI;
    double delta = asin(z_equat / r) * 180.0 / M_PI;

    double uth = fractional_day * 24.0; // hours
    double gmst0 = fmod(L + 180, 360.0)/15.0;
    double sid_time = gmst0 + uth + lon/15.0;
    double HA = sid_time * 15.0 - RA;

           // Precompute sin/cos of latitude if not provided
    if (std::isnan(sin_lat) || std::isnan(cos_lat)) {
        sin_lat = sin(deg_to_rad(lat));
        cos_lat = cos(deg_to_rad(lat));
    }

    double xhor = cos(deg_to_rad(HA)) * cos(deg_to_rad(delta));
    double zhor = sin_lat * sin(deg_to_rad(delta)) + cos_lat * cos(deg_to_rad(delta)) * cos(deg_to_rad(HA));
    return rad_to_deg(asin(zhor));
}

int NightTime::calculateNightTime(int dept_airport_id, int dest_airport_id,
                                        int departure_time_ms, int flight_time_minutes,
                                        double night_angle) {
    // Retrieve coordinates from airportGeoData
    double dept_lat = airportGeoData->latitude(dept_airport_id);
    double dept_lon = airportGeoData->longitude(dept_airport_id);
    double dest_lat = airportGeoData->latitude(dest_airport_id);
    double dest_lon = airportGeoData->longitude(dest_airport_id);

           // Decide number of steps for granularity
    const int num_steps = flight_time_minutes; // 1 step per minute
    auto points = intermediate_points_on_great_circle(dept_lat, dept_lon, dest_lat, dest_lon, num_steps);

    int night_time_ms = 0;

    int step_time_ms = (num_steps > 1)
                           ? (flight_time_minutes * ONE_MINUTE_MS / (num_steps - 1))
                           : (flight_time_minutes * ONE_MINUTE_MS);

           // Iterate in departure → arrival order
    for (int i = 0; i < points.size(); ++i)
    {
        double lat = points[i].first;
        double lon = points[i].second;

        DEB << "New: lat/long" << lat << " / " << lon;

        int time_ms = departure_time_ms + i * step_time_ms;

        if (solarElevation(time_ms, lat, lon) < night_angle)
            night_time_ms += step_time_ms;
    }

           // Clamp to total flight duration in case of rounding
    int total_flight_ms = flight_time_minutes * ONE_MINUTE_MS;
    if (night_time_ms > total_flight_ms)
        night_time_ms = total_flight_ms;

    return night_time_ms;
}

bool NightTime::isNight(int airport_id, int time_ms_since_start_of_day, double night_angle) {
    double lat = airportGeoData->latitude(airport_id);
    double lon = airportGeoData->longitude(airport_id);
    double sin_lat = sin(deg_to_rad(lat));
    double cos_lat = cos(deg_to_rad(lat));
    DEB << "Elevation at " << QTime::fromMSecsSinceStartOfDay(time_ms_since_start_of_day) << ": " <<solarElevation(time_ms_since_start_of_day, lat, lon, sin_lat, cos_lat);
    return solarElevation(time_ms_since_start_of_day, lat, lon, sin_lat, cos_lat) < night_angle;
}
*/
