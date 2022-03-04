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
#include "ahash.h"
#include "src/opl.h"

AHash::AHash(QFileInfo &file_info)
{
    QFile f(file_info.absoluteFilePath());
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        if (hash.addData(&f)) {
            checksum = hash.result();
            //DEB << "File: " << f.fileName();
            //DEB << "Hash: " << hash.result().toHex();
        } else {
            checksum = QByteArray();
        }
    } else {
        checksum = QByteArray();
    }
    f.close();
}

bool AHash::compare(QFileInfo &md5_file)
{
    // Open the file and read the first 32 characters
    QFile f(md5_file.absoluteFilePath());
    if (f.open(QFile::ReadOnly)) {
        QTextStream in(&f);
        const QString hash_string = in.read(32);

        // Verify checksum is not empty and compare to md5 read from file
        if (checksum == QByteArray())
            return false;
        else
            if (checksum.toHex() == hash_string)
                return true;
            else {
                LOG << QString("Checksum for %1 invalid. Calculated: %2 - Hash: %3").arg(f.fileName(),
                                                                                         QString(checksum.toHex()),
                                                                                         hash_string);
                return false;
            }
    }
    return false;
}
