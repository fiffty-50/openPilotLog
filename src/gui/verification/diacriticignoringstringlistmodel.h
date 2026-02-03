#ifndef DIACRITICIGNORINGSTRINGLISTMODEL_H
#define DIACRITICIGNORINGSTRINGLISTMODEL_H

#include <QRegularExpression>
#include <QStringListModel>

class DiacriticIgnoringStringListModel : public QStringListModel {
  public:
    using QStringListModel::QStringListModel;

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == DiacriticFreeRole) {
            QString value = QStringListModel::data(index, Qt::DisplayRole).toString();
            return normalise(value);
        }
        else {
            return QStringListModel::data(index, role);
        }
    }

  private:
    constexpr static int DiacriticFreeRole = Qt::UserRole + 10;

    static QString normalise(const QString &input)
    {
        QString r = input.normalized(QString::NormalizationForm_D);
        static QRegularExpression diacritics("\\p{Mn}");
        r.remove(diacritics);
        r.remove(QRegularExpression("[^A-Za-z0-9]"));
        return r.toLower();
    }
};

#endif // DIACRITICIGNORINGSTRINGLISTMODEL_H
