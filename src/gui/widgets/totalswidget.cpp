#include "totalswidget.h"
#include "ui_totalswidget.h"
#include "src/testing/adebug.h"

TotalsWidget::TotalsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TotalsWidget)
{
    ui->setupUi(this);
    auto data = AStat::totals();
    DEB "Filling Totals Line Edits...";
    //DEB "data: " << data;
    for (const auto &field : data) {
        auto line_edit = parent->findChild<QLineEdit *>(field.first + "LineEdit");
        line_edit->setText(field.second);
    }
    QSettings settings;
    QString name = settings.value("userdata/firstname").toString();
    if(!name.isEmpty()) {
        QString salutation = "Welcome to openPilotLog, " + name + QLatin1Char('!');
        ui->welcomeLabel->setText(salutation);
    }
}

TotalsWidget::~TotalsWidget()
{
    delete ui;
}
