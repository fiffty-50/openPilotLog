#include "totalswidget.h"
#include "ui_totalswidget.h"
// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

totalsWidget::totalsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::totalsWidget)
{
    ui->setupUi(this);
    auto data = stat::totals();
    DEB("Filling Totals Line Edits...");
    for (const auto &field : data) {
        auto line_edit = parent->findChild<QLineEdit *>(field.first + "LineEdit");
        line_edit->setText(field.second);
    }
}

totalsWidget::~totalsWidget()
{
    delete ui;
}
