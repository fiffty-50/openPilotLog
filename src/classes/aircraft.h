#ifndef AIRCRAFT_H
#define AIRCRAFT_H
#include <QCoreApplication>
#include <QVector>
#include "src/database/db.h"

class aircraft
{
public:
    aircraft();

    QString id;
    QString registration;
    QString company;
    QString make;
    QString model;
    QString variant;
    bool singlepilot;
    bool multipilot;
    bool singleengine;
    bool multiengine;
    bool unpowered;
    bool piston;
    bool turboprop;
    bool jet;
    bool light;
    bool medium;
    bool heavy;

    // Functions
    static aircraft         fromTails(int tail_id);
    static aircraft         fromAircraft(int acft_id);
    static QVector<QString> toVector(aircraft);

    // Debug functionality
    void print();
    QString debug();
    operator QString() { return debug(); }
};

#endif // AIRCRAFT_H
