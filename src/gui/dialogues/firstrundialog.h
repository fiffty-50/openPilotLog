#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QMessageBox>
#include <QStringBuilder>

namespace Ui {
class FirstRunDialog;
}

class FirstRunDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FirstRunDialog(QWidget *parent = nullptr);
    ~FirstRunDialog();

private slots:

    void on_previousPushButton_clicked();

    void on_nextPushButton_clicked();

private:
    Ui::FirstRunDialog *ui;

    void reject() override;
    bool setupDatabase();
    bool finish();
    bool useLocalTemplates;

};

#endif // FIRSTRUNDIALOG_H
