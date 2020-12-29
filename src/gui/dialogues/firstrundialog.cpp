#include "firstrundialog.h"
#include "ui_firstrundialog.h"
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/database/adatabasesetup.h"
#include "src/classes/apilotentry.h"
#include "src/classes/adownload.h"
#include "src/classes/asettings.h"
const auto TEMPLATE_URL = QLatin1String("https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/assets/database/templates/");

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->lastnameLineEdit->setFocus();
    ui->nightComboBox->setCurrentIndex(1);

    auto *themeGroup = new QButtonGroup;
    themeGroup->addButton(ui->systemThemeCheckBox, 0);
    themeGroup->addButton(ui->lightThemeCheckBox, 1);
    themeGroup->addButton(ui->darkThemeCheckBox, 2);

    QObject::connect(themeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
                     this, &FirstRunDialog::on_themeGroup_toggled);
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::on_previousPushButton_clicked()
{
    if(ui->tabWidget->currentIndex()>0)
        ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()-1);
}

void FirstRunDialog::on_nextPushButton_clicked()
{
    if(ui->tabWidget->currentIndex()<2){
        ui->tabWidget->setCurrentIndex(ui->tabWidget->currentIndex()+1);
    } else {
        emit ui->finishButton->clicked();
    }

}

void FirstRunDialog::on_themeGroup_toggled(int id)
{
    ASettings::write("main/theme", id);
}

void FirstRunDialog::on_finishButton_clicked()
{
    if(ui->lastnameLineEdit->text().isEmpty() || ui->firstnameLineEdit->text().isEmpty()){
        auto mb = new QMessageBox(this);
        mb->setText("You have to enter a valid first and last name for the logbook.");
        mb->show();
    } else {
        ASettings::write("userdata/lastname", ui->lastnameLineEdit->text());
        ASettings::write("userdata/firstname", ui->firstnameLineEdit->text());
        ASettings::write("userdata/employeeid", ui->employeeidLineEdit->text());
        ASettings::write("userdata/phone", ui->phoneLineEdit->text());
        ASettings::write("userdata/email", ui->emailLineEdit->text());

        ASettings::write("flightlogging/function", ui->functionComboBox->currentText());
        ASettings::write("flightlogging/approach", ui->approachComboBox->currentText());
        ASettings::write("flightlogging/nightlogging", ui->nightComboBox->currentIndex());
        ASettings::write("flightlogging/logIfr", ui->rulesComboBox->currentIndex());
        ASettings::write("flightlogging/flightnumberPrefix", ui->prefixLineEdit->text());

        ASettings::write("flightlogging/numberTakeoffs", 1);
        ASettings::write("flightlogging/numberLandings", 1);
        ASettings::write("flightlogging/popupCalendar", true);
        ASettings::write("flightlogging/pilotFlying", true);


        QMap<QString, QVariant> data;
        switch (ui->aliasComboBox->currentIndex()) {
        case 0:
            ASettings::write("userdata/displayselfas", ui->aliasComboBox->currentIndex());
            break;
        case 1:
            ASettings::write("userdata/displayselfas", ui->aliasComboBox->currentIndex());
            break;
        case 2:{
            ASettings::write("userdata/displayselfas", ui->aliasComboBox->currentIndex());
        }
            break;
        default:
            break;
        }
        data.insert("lastname", ui->lastnameLineEdit->text());
        data.insert("firstname", ui->firstnameLineEdit->text());
        data.insert("alias", "self");
        data.insert("employeeid", ui->employeeidLineEdit->text());
        data.insert("phone", ui->phoneLineEdit->text());
        data.insert("email", ui->emailLineEdit->text());

        if (!finishSetup()) {
            QMessageBox message_box(this);
            message_box.setText("Errors have ocurred creating the database. Without a working database The application will not be usable.");
        }
        ASettings::write("setup/setup_complete", true);
        aDB()->disconnect(); // reset db connection to refresh layout after initial setup.
        aDB()->connect();
        auto pilot = APilotEntry(1);
        pilot.setData(data);
        if (aDB()->commit(pilot)) {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        } else {
            QMessageBox message_box(this);
            message_box.setText("Errors have ocurred creating the database. Without a working database The application will not be usable.");
        }

    }
}

bool FirstRunDialog::finishSetup()
{

    //check if template dir exists and create if needed.
    QDir dir("data/templates");
    DEB dir.path();
    if (!dir.exists())
        dir.mkpath(".");

    QMessageBox confirm;
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::No);
    confirm.setIcon(QMessageBox::Question);
    confirm.setWindowTitle("Create Database");
    confirm.setText("We are now going to create the database. Would you like to download the latest database information?\n(Recommended, Internet connection required)\n");
    int reply = confirm.exec();
    if (reply == QMessageBox::Yes) {
        // download latest csv
        QStringList templateTables = {"aircraft", "airports", "changelog"};
        QString linkStub = TEMPLATE_URL;
        for (const auto& table : templateTables) {
            QEventLoop loop;
            ADownload* dl = new ADownload;
            connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
            dl->setTarget(QUrl(linkStub + table + ".csv"));
            dl->setFileName("data/templates/" + table + ".csv");
            dl->download();
            loop.exec(); // event loop waits for download done signal before allowing loop to continue
            dl->deleteLater();
        }
    }

    //close database connection
    aDB()->disconnect();

    // back up old database
    auto oldDatabase = QFile("data/logbook.db");
    if (oldDatabase.exists()) {
        auto dateString = QDateTime::currentDateTime().toString(Qt::ISODate);
        DEB "Backing up old database as: " << "logbook-backup-" + dateString;
        if (!oldDatabase.rename("data/logbook-backup-" + dateString)) {
            DEB "Warning: Creating backup of old database has failed.";
        }
    }
    // re-connect and create new database
    aDB()->connect();

    if (ADataBaseSetup::createDatabase()) {
        if (ADataBaseSetup::importDefaultData()) {
            ASettings::write("setup/setup_complete", true);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


