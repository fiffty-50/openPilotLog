#ifndef DIACRITICIGNORINGCOMPLETER_H
#define DIACRITICIGNORINGCOMPLETER_H

#include "diacriticignoringstringlistmodel.h"
#include <QCompleter>
#include <QRegularExpression>

/*!
 * \brief The DiacriticIgnoringCompleter class is used for completion of strings containing diacritics
 * \details This QCompleter is used for two cases:
 * <ul>
 * <li> Matching aircraft registrations that contain optional '-' characters </li>
 * </li> Matching names that contain diacritics </li>
 * <ul>
 *
 * It is used together with the DiacriticIgnoringStringListModel to enable completion
 * without having to type out diacritic characters.
 *
 * For the completer to work as intended the complionRole and model must be set correctly.
 * It is recommended to use the static builder function getCompleter() to create a new
 * instance.
 *
 */
class DiacriticIgnoringCompleter : public QCompleter
{
public:
    using QCompleter::QCompleter;
    DiacriticIgnoringCompleter(const QStringList &completions, QObject *parent = nullptr)
      : QCompleter(completions, parent)    {
      setCompletionRole(Qt::UserRole + 10);
    }

    QString pathFromIndex(const QModelIndex &index) const {
    // needed to use original value when value is selected
    return index.data().toString();

    }
    QStringList splitPath(const QString &path) const {
        return { normalise(path) };
    }

    static QString normalise(const QString &input) {
        QString r = input.normalized(QString::NormalizationForm_D);
        static QRegularExpression diacritics("\\p{Mn}");
        r.remove(diacritics);
        r.remove(QRegularExpression("[^A-Za-z0-9]"));
        return r.toLower();
    }

    static constexpr int diacriticIgnoringRole() {
        return Qt::UserRole + 10;
    }

    static DiacriticIgnoringCompleter *createCompleter(const QStringList &completion_list, QObject *parent = nullptr)  {
        auto completer = new DiacriticIgnoringCompleter(parent);
        completer->setCompletionRole(diacriticIgnoringRole());
        auto model = new DiacriticIgnoringStringListModel(completion_list);
        completer->setModel(model);
        return completer;
    }
};

#endif // DIACRITICIGNORINGCOMPLETER_H
