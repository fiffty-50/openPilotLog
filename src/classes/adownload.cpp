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
#include "adownload.h"
#include "src/functions/alog.h"




ADownload::ADownload() : QObject(nullptr)
{
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(downloadFinished(QNetworkReply*)));
}

ADownload::~ADownload()
{
    DEB << "Deleting ADownload Object";
}

void ADownload::setTarget(const QUrl &value)
{
    this->target = value;
}

void ADownload::setFileName(const QString &value)
{
    this->fileName = value;
}

void ADownload::download()
{
    QNetworkRequest request(target);
    DEB << "Downloading from: " << target.toString();

    QObject::connect(manager.get(request), SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
}


void ADownload::downloadProgress(qint64 received, qint64 total)
{
    DEB << "Received " << received << " bytes of " << total;
}



void ADownload::downloadFinished(QNetworkReply *data)
{
    QFile localFile(fileName);
    if (!localFile.open(QIODevice::WriteOnly))
        return;
    const QByteArray sdata = data->readAll();
    localFile.write(sdata);
    localFile.close();
    DEB << "Download finished. Output file: " << fileName << "size: " << localFile.size();

    emit done();
}

/* usage:
 *
 *  auto dl = new Download(); // QNetworkAccessManager is asynchronous, so it needs an event loop to do any downloading
 *  dl->setTarget(QUrl("https://raw.githubusercontent.com/fiffty-50/openpilotlog/master/README.md"));
 *  dl->setFileName("out.md");
 *  dl->download();
 *
 *  //do whatever when the download is done.
 *  QObject::connect(&dl, SIGNAL(done()), &a, SLOT(whatever()));
 *
 */
