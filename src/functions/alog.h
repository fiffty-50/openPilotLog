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
#ifndef ALOG_H
#define ALOG_H

#include <QDebug>

/* Use QTextStream to print status messages. These messages
 * are oriented towards end-users and will persist even when
 * compiling a release without enabling qDebug. As such, LOG
 * should be used to document "important" enough steps that
 * the user would care about or that are "milestones" that
 * the program runs through.
 */
#define LOG QTextStream(stdout) << "openPilotLog - "

#endif // ALOG_H
