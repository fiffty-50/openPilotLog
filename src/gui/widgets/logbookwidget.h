#ifndef LOGBOOKWIDGET_H
#define LOGBOOKWIDGET_H

#include "src/gui/dialogues/entryeditdialog.h"
#include "src/gui/views/logbookview.h"
#include "src/gui/widgets/logbookwidgetui.h"
#include "src/gui/widgets/settingswidget.h"
#include "src/opl.h"
#include <QModelIndex>
#include <QWidget>

class LogbookWidget : public QWidget {
    Q_OBJECT
  public:
    explicit LogbookWidget(QWidget *parent = nullptr);
    ~LogbookWidget() { delete ui; }

  private:
    // Ui
    Ui::LogbookWidgetUi *ui = nullptr;
    std::unique_ptr<LogbookViewInfo> m_viewHelper;

    // Dialog Setup
    void init();
    void setupModelAndView();
    void setupSignals();

    // Member Variables
    OPL::LogbookView m_logbookView;

    // Edit Dialogues
    EntryEditDialog *m_flightEditDialog;
    EntryEditDialog *m_simEditDialog;

    //
    struct Event {
        OPL::LogEvent type;
        int rowId;
    };

    QString confirmDeleteString(const QModelIndex &index);
    QString errorOnDeleteString(const QModelIndex &index);
    QString summaryString(const QModelIndex &index);

    bool setViewHelper();
    //QString getFilterStatement(const QString &column, const QString &filterText);

  public slots:
    void viewSelectionChanged(SettingsWidget::SettingSignal widget);
  private slots:
    void openEntryEdit(const QModelIndex &index);
    void deleteEntryRequested(const QModelIndex &index);
    //void filterTextChanged(const QString &filterText);
};

#endif // LOGBOOKWIDGET_H
