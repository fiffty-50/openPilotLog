#include "homewidget.h"
#include "ui_homewidget.h"

homeWidget::homeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::homeWidget)
{
    ui->setupUi(this);
    ui->totalTimeThisYearDisplay->setText(
                calc::minutes_to_string(
                stat::totalTime(stat::calendarYear)));
    ui->totalTime365DaysDisplay->setText(
                calc::minutes_to_string(
                stat::totalTime(stat::rollingYear)));
    QVector<QString> toldg = stat::currencyTakeOffLanding(90);
    QString ToLdg = toldg[0] + " / " + toldg[1];
    ui->ToLdgDisplay->setText(ToLdg);
}

homeWidget::~homeWidget()
{
    delete ui;
}

void homeWidget::on_pushButton_clicked()
{
    //auto nt = new NewTail(this);
    //nt->show();
    //qDebug() <<
    auto ac = aircraft::fromTails(1);
    qDebug() << ac;
}
