#ifndef DEFAULTLOGBOOKVIEW_H
#define DEFAULTLOGBOOKVIEW_H

#include <QTableView>

class DefaultLogbookView : public QTableView
{
public:
    DefaultLogbookView(QWidget *parent = nullptr);
private:
    const static int COL_ID   = 0;
    const static int COL_DATE = 1;
    const static int COL_DEPT = 2;
    const static int COL_DEST = 3;
    const static int COL_TOFB = 4;
    const static int COL_TONB = 5;
    const static int COL_PIC  = 6;
    const static int COL_ACFT = 7;
    const static int COL_TBLK = 8;
    const static int COL_TSPSE = 9;
    const static int COL_TSPME = 10;
    const static int COL_TMP = 11;
    const static int COL_TNIGHT = 12;
    const static int COL_TIFR = 13;
    const static int COL_TPIC = 14;
    const static int COL_TPICUS = 15;
    const static int COL_TSIC = 16;
    const static int COL_TDUAL = 17;
    const static int COL_TFI = 18;
    const static int COL_TSIM = 19;// do not use, use sim table TODO: remove from DB
    const static int COL_PF = 20;
    const static int COL_TO_DAY = 21;
    const static int COL_TO_NIGHT = 22;
    const static int COL_LDG_DAY = 23;
    const static int COL_LDG_NIGHT = 24;
    const static int COL_AUTOLAND = 25;
    const static int COL_SECOND_PILOT = 26;
    const static int COL_THIRD_PILOT = 27;
    const static int COL_APP_TYPE = 28;
    const static int COL_FLIGHT_NR = 29;
    const static int COL_REMARKS = 30;

    const static int COLUMN_COUNT = 31;



};

#endif // DEFAULTLOGBOOKVIEW_H
