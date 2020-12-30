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
#ifndef ASETTINGS_H
#define ASETTINGS_H
#include <QtCore>
#include <QSettings>

/*!
 * \brief Thin (namespace) wrapper for the QSettings class,
 * simplifying reading and writing of settings.
 */
namespace ASettings
{

/*!
 * \brief Should be called after QCoreApplication::set...Name have been called.
 */
void setup();

QVariant read(const QString &key);

void write(const QString &key, const QVariant &val);

QSettings settings();

}

#endif // ASETTINGS_H
