#include "totalswidget.h"
#include "ui_totalswidget.h"
// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

TotalsWidget::TotalsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TotalsWidget)
{
    ui->setupUi(this);
    auto data = Stat::totals();
    DEB("Filling Totals Line Edits...");
    for (const auto &field : data) {
        auto line_edit = parent->findChild<QLineEdit *>(field.first + "LineEdit");
        line_edit->setText(field.second);
    }
}

TotalsWidget::~TotalsWidget()
{
    delete ui;
}
