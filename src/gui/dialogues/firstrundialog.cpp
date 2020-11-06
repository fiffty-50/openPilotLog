#include "firstrundialog.h"
#include "ui_firstrundialog.h"

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);

    disregard();
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::on_pushButton_clicked()
{
    accept();
}

void FirstRunDialog::disregard()
{
    //emit ui->pushButton->clicked();

}
