/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "aircraftentryeditdialog.h"
#include "src/database/database.h"
#include "src/opl.h"

AircraftEntryEditDialog::AircraftEntryEditDialog(QWidget *parent) : EntryEditDialog(0, parent)
{
    init();
}

AircraftEntryEditDialog::AircraftEntryEditDialog(int row_id, QWidget *parent)
    : EntryEditDialog(row_id, parent)
{
    init();
    loadAircraftData(row_id);
}

void AircraftEntryEditDialog::init()
{
    // Main Layout
    gridLayout                  = new QGridLayout(this);
    int row                     = 0;
    constexpr int firstCol      = 0;
    constexpr int secondCol     = 1;
    constexpr int singleSpan    = 1;
    constexpr int spanRemaining = -1;

    // Add widgets to first and second column, advance to next row
    auto addTwoWidgets = [&](QWidget *left, QWidget *right) {
        gridLayout->addWidget(left, row, firstCol, singleSpan, singleSpan);
        gridLayout->addWidget(right, row, secondCol, singleSpan, spanRemaining);
        row++;
    };

    // Row
    makeLabel    = new QLabel(this);
    makeLineEdit = new QLineEdit(this);
    addTwoWidgets(makeLabel, makeLineEdit);

    // Row
    modelLabel    = new QLabel(this);
    modelLineEdit = new QLineEdit(this);
    addTwoWidgets(modelLabel, modelLineEdit);

    // Row
    variantLabel    = new QLabel(this);
    variantLineEdit = new QLineEdit(this);
    addTwoWidgets(variantLabel, variantLineEdit);

    // Row
    icaoDesignatorLabel    = new QLabel(this);
    icaoDesignatorLineEdit = new QLineEdit(this);
    addTwoWidgets(icaoDesignatorLabel, icaoDesignatorLineEdit);

    // Row
    engineTypeLabel    = new QLabel(this);
    engineTypeComboBox = new QComboBox(this);
    addTwoWidgets(engineTypeLabel, engineTypeComboBox);

    // Row
    multiEngineLabel    = new QLabel(this);
    multiEngineComboBox = new QComboBox(this);
    addTwoWidgets(multiEngineLabel, multiEngineComboBox);

    // Row
    multiPilotLabel    = new QLabel(this);
    multiPilotComboBox = new QComboBox(this);
    addTwoWidgets(multiPilotLabel, multiPilotComboBox);

    // Row
    classLabel    = new QLabel(this);
    classComboBox = new QComboBox(this);
    addTwoWidgets(classLabel, classComboBox);

    // Row
    subClassLabel    = new QLabel(this);
    subClassComboBox = new QComboBox(this);
    addTwoWidgets(subClassLabel, subClassComboBox);

    // Row
    wakeCategoryLabel    = new QLabel(this);
    wakeCategoryComboBox = new QComboBox(this);
    addTwoWidgets(wakeCategoryLabel, wakeCategoryComboBox);

    // Row
    typeRatingLabel    = new QLabel(this);
    typeRatingLineEdit = new QLineEdit(this);
    addTwoWidgets(typeRatingLabel, typeRatingLineEdit);

    // Row
    remarksLabel    = new QLabel(this);
    remarksTextEdit = new QPlainTextEdit(this);
    addTwoWidgets(remarksLabel, remarksTextEdit);

    // Row
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel |
                                  QDialogButtonBox::StandardButton::Ok);
    gridLayout->addWidget(buttonBox, row, firstCol, singleSpan, spanRemaining);

    retranslateUi();
    setupSlots();
}

void AircraftEntryEditDialog::retranslateUi()
{

    m_rowId == 0 ? this->setWindowTitle(tr("Add New Aircraft"))
                 : this->setWindowTitle(tr("Edit Aircraft"));

    makeLabel->setText(tr("Make"));
    modelLabel->setText(tr("Model"));
    variantLabel->setText(tr("Variant"));
    icaoDesignatorLabel->setText(tr("ICAO Designator"));
    engineTypeLabel->setText(tr("Engine Type"));
    multiEngineLabel->setText(tr("Engine Count"));
    classLabel->setText(tr("Class"));
    subClassLabel->setText(tr("Sub Class"));
    typeRatingLabel->setText(tr("Type Rating"));
    remarksLabel->setText(tr("Remarks"));

    OPL::AircraftEntry::setupEngineTypeComboBox(engineTypeComboBox);
    OPL::AircraftEntry::setupAircraftClassComboBox(classComboBox);
    OPL::AircraftEntry::setupAircraftSubClassComboBox(subClassComboBox);
    OPL::AircraftEntry::setupIsMultiEngineComboBox(multiEngineComboBox);
    OPL::AircraftEntry::setupWakeCategoryComboBox(wakeCategoryComboBox);
    OPL::AircraftEntry::setupIsMultiPilotComboBox(multiPilotComboBox);
}

void AircraftEntryEditDialog::setupSlots()
{
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this,
                     &AircraftEntryEditDialog::on_accepted);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void AircraftEntryEditDialog::loadAircraftData(int rowId)
{
    const auto entry = DB->getAircraftEntry(rowId);
    DEB << "Loading aircraft entry data for row ID:" << rowId;
    DEB << entry;

    makeLineEdit->setText(entry.getMake());
    modelLineEdit->setText(entry.getModel());
    variantLineEdit->setText(entry.getVariant());
    icaoDesignatorLineEdit->setText(entry.getIcaoDesignator());
    typeRatingLineEdit->setText(entry.getTypeRating());
    remarksTextEdit->setPlainText(entry.getRemarks());

    DEB << "Setting is multi-engine to:" << entry.getIsMultiEngine();
    multiEngineComboBox->setCurrentIndex(entry.getIsMultiEngine() ? 1 : 0);
    multiPilotComboBox->setCurrentIndex(entry.getIsMultiPilot() ? 1 : 0);

    OPL::AircraftEntry::EngineType engineType = entry.getEngineType();
    {
        int engineTypeIndex = engineTypeComboBox->findData(static_cast<int>(engineType));
        if (engineTypeIndex == -1) {
            LOG << QStringLiteral("Engine type not found in combo box.");
        }
        engineTypeComboBox->setCurrentIndex(engineTypeIndex);
    }

    OPL::AircraftEntry::AircraftClass aircraftClass = entry.getClass();
    {
        int classIndex = classComboBox->findData(static_cast<int>(aircraftClass));
        if (classIndex == -1) {
            LOG << QStringLiteral("Aircraft class not found in combo box.");
        }
        classComboBox->setCurrentIndex(classIndex);
    }

    OPL::AircraftEntry::AircraftSubClass aircraftSubClass = entry.getSubClass();
    {
        int subClassIndex = subClassComboBox->findData(static_cast<int>(aircraftSubClass));
        if (subClassIndex == -1) {
            LOG << QStringLiteral("Aircraft subclass not found in combo box.");
        }
        subClassComboBox->setCurrentIndex(subClassIndex);
    }

    OPL::AircraftEntry::WakeCategory wakeCategory = entry.getWakeCategory();
    {
        int wakeCategoryIndex = wakeCategoryComboBox->findData(static_cast<int>(wakeCategory));
        if (wakeCategoryIndex == -1) {
            LOG << QStringLiteral("Wake category not found in combo box.");
        }
        wakeCategoryComboBox->setCurrentIndex(wakeCategoryIndex);
    }
}

void AircraftEntryEditDialog::on_accepted()
{
    auto entry = OPL::AircraftEntry();
    entry.setRowId(m_rowId);

    if (!entry.setMake(makeLineEdit->text())) {
        WARN(tr("Unable to set aircraft make (manufacturer)<br><br>The following error has "
                "occurred:<br>%1")
                 .arg(DB->lastError.text()));
    }
    if (!entry.setModel(modelLineEdit->text())) {
        WARN(tr("Unable to set aircraft model (required)<br><br>The following error has "
                "occurred:<br>%1")
                 .arg(DB->lastError.text()));
    }
    entry.setVariant(variantLineEdit->text());
    entry.setIcaoDesignator(icaoDesignatorLineEdit->text());
    entry.setTypeRating(typeRatingLineEdit->text());
    entry.setRemarks(remarksTextEdit->toPlainText());

    entry.setIsMultiEngine(multiEngineComboBox->currentIndex() == 1);
    entry.setIsMultiPilot(multiPilotComboBox->currentIndex() == 1);

    OPL::AircraftEntry::EngineType engineType =
        static_cast<OPL::AircraftEntry::EngineType>(engineTypeComboBox->currentData().toInt());
    entry.setEngineType(engineType);

    OPL::AircraftEntry::AircraftClass aircraftClass =
        static_cast<OPL::AircraftEntry::AircraftClass>(classComboBox->currentData().toInt());
    entry.setClass(aircraftClass);

    OPL::AircraftEntry::AircraftSubClass aircraftSubClass =
        static_cast<OPL::AircraftEntry::AircraftSubClass>(subClassComboBox->currentData().toInt());
    entry.setSubClass(aircraftSubClass);

    OPL::AircraftEntry::WakeCategory wakeCategory =
        static_cast<OPL::AircraftEntry::WakeCategory>(wakeCategoryComboBox->currentData().toInt());
    entry.setWakeCategory(wakeCategory);

    DEB << "Saving aircraft entry:" << entry.getPosition();
    DEB << entry;

    if (!DB->commit(entry)) {
        WARN(tr("Unable to save aircraft entry to database<br><br>The following error has "
                "occurred:<br>%1")
                 .arg(DB->lastError.text()));
        return;
    } else {
        m_rowId = DB->getLastEntry(OPL::DbTable::Aircraft);
        QDialog::accept();
    }
}

// EntryEdit interface
void AircraftEntryEditDialog::loadEntry(int rowID)
{
    loadAircraftData(rowID);
    m_rowId = rowID;
}

bool AircraftEntryEditDialog::deleteEntry(int rowID)
{
    return DB->remove(OPL::DbTable::v2AircraftTypes, rowID);
}
