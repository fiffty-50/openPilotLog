#include "aircraft.h"

aircraft::aircraft()
{

}

aircraft aircraft::fromTails(int tail_id)
{
    QVector<QString> columns = {
        "tail_id", "registration", "company",
        "make", "model", "variant",
        "singlepilot", "multipilot", "singleengine", "multiengine",
        "unpowered", "piston", "turboprop",
        "jet", "light", "medium", "heavy"
    };
    auto tail = db::multiSelect(columns,"tails","tail_id",QString::number(tail_id),sql::exactMatch);

    qDebug() << tail;
    aircraft acft;

    acft.id = tail[0];
    acft.registration = tail[1];
    acft.company = tail[2];
    acft.make = tail[3];
    acft.model = tail[4];
    acft.variant = tail[5];
    //bool
    acft.singlepilot = tail[6].toInt();
    acft.multipilot = tail[7].toInt();
    acft.singleengine = tail[8].toInt();
    acft.multiengine = tail[9].toInt();
    acft.unpowered = tail[10].toInt();
    acft.piston = tail[11].toInt();
    acft.turboprop = tail[12].toInt();
    acft.jet = tail[13].toInt();
    acft.light = tail[14].toInt();
    acft.medium = tail[15].toInt();
    acft.heavy = tail[16].toInt();

    return acft;
}

void aircraft::print()
{
    QTextStream cout(stdout, QIODevice::WriteOnly);

    cout << "\t\033[38;2;0;255;0;48;2;0;0;0m Aircraft Object: \033[0m\n";
    cout << "ID: \t\t" << id << "\n";
    cout << "Reg: \t\t" << registration<< "\n";
    cout << "Company: \t" << company<< "\n";
    cout << "Type:\t\t" << make << " " << model << " " <<  variant<< "\n";
    cout << "SP:\t\t" << singlepilot << "\tMP:\t" << multipilot << "\t"
         << "SE:\t" << singleengine << "\tME:\t" << multiengine << "\n";
    cout << "UNP:\t\t" << unpowered << "\tPIS:\t" << piston << "\t"
         << "TPR:\t" << turboprop << "\tJET:\t" << jet << "\n";
    cout << "Light:\t" << light << "\tMedium:\t" << medium << "\tHeavy:\t" << heavy << "\n";

}

QString aircraft::debug()
{
    print();
    return QString();
}
