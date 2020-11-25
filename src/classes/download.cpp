/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#include "download.h"
#include "debug.h"




Download::Download() : QObject(nullptr)
{
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(downloadFinished(QNetworkReply*)));
}

Download::~Download()
{
    DEB("Deleting Download object");
}

void Download::setTarget(const QUrl &value)
{
    this->target = value;
}

void Download::setFileName(const QString &value)
{
    this->fileName = value;
}

void Download::download()
{
    QNetworkRequest request(target);
    DEB("Downloading from: " << target.toString());

    QObject::connect(manager.get(request), SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
}


void Download::downloadProgress(qint64 received, qint64 total)
{
    auto shutupcompilerwarning = received;
    shutupcompilerwarning += total;
    //qDebug() << "Received " << received << " out of " << total;
}

void Download::downloadFinished(QNetworkReply *data)
{
    QFile localFile(fileName);
    if (!localFile.open(QIODevice::WriteOnly))
        return;
    const QByteArray sdata = data->readAll();
    localFile.write(sdata);
    localFile.close();
    qDebug() << "Download finished. Output file: " << fileName;

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
