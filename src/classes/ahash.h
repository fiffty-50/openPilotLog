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
#ifndef AHASH_H
#define AHASH_H
#include <QtCore>
#include <QByteArray>
#include <QCryptographicHash>

/*!
 * \brief The AHash class is responsible for calculating cryptographic hashes of files (used to verify downloads)
 */
class AHash
{
public:
    /*!
     * \brief AHash - calculates the MD5-checksum for the parameter given in the constructor and
     * saves the result in the checksum member variable
     */
    AHash(QFileInfo &file_info);

    QByteArray checksum;

    /*!
     * \brief hashString returns a hex representation of the hash
     */
    inline const QString hashToHex()
    {
            return QString(checksum.toHex());
    };

    /*!
     * \brief compare reads the pre-calculated md5-sum from a checkfile and compares the hashes.
     * \param md5_file - the checkfile containing the md5 checksum in hex format
     * \return true if hashes match
     */
    bool compare(QFileInfo &md5_file);
};

#endif // AHASH_H