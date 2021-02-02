/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#ifndef ADOWNLOAD_H
#define ADOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QFile>
#include <QDebug>


class ADownload : public QObject {
    Q_OBJECT
public:
    explicit ADownload();

    ~ADownload();

    void setTarget(const QUrl &value);

    void setFileName(const QString &value);

    void download();

private:

    QNetworkAccessManager manager;
    QUrl target;
    QString fileName;

signals:
    void done();

public slots:
    void downloadFinished(QNetworkReply* data);
    void downloadProgress(qint64 received, qint64 total);
};

#endif // ADOWNLOAD_H
