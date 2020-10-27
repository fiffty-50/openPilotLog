#include "newtail.h"
#include "ui_newtail.h"

NewTail::NewTail(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTail)
{
    ui->setupUi(this);

    auto cl = new completionList(completerTarget::aircraft);
    aircraftlist = cl->list;

    QCompleter* completer = new QCompleter(aircraftlist,ui->searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    ui->searchLineEdit->setCompleter(completer);

}

NewTail::~NewTail()
{
    delete ui;
}


void NewTail::on_searchLineEdit_textChanged(const QString &arg1)
{
    if(aircraftlist.contains(arg1)){
        qDebug() << "Success!" << ui->searchLineEdit->text();
        //call autofiller for dialog
        ui->searchLineEdit->setStyleSheet("border: 1px solid green");
    }else{
        ui->searchLineEdit->setStyleSheet("border: 1px solid orange");
    }
}
