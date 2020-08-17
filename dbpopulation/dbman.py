#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 22 20:04:38 2020
Used to create the initial database layout.
@author: Felix Turowsky
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


"""
This set of functions is used to initialise or reset the database.
"""

import sqlite3
import os


def initialisedb(): # For first run only, creates all databases.
    createdbairports()
    createdbflights()


def createdbflights(): #this database will hold flight information
    conn = sqlite3.connect(os.path.join('./resources', 'flog.db'))
    c = conn.cursor()
    c.execute('''create table flights (
            id integer primary key,
            doft numeric not null,
            dept text not null,
            tofb integer not null,
            dest text not null,
            tonb integer not null,
            tblk integer,
            pic integer,
            acft integer,
            foreign key (pic) references pilots (pilot_id)
            foreign key (acft) references tails (tail_id)
            )
            ''')
    conn.close()


def deletedbflights(): # deletes flights database. Use with caution!
    conn = sqlite3.connect(os.path.join('./resources', 'flog.db'))
    c = conn.cursor()
    c.execute("DROP TABLE flights")
    print('Table flights has been deleted')
    conn.close()


def createdbairports(): # this database will hold airport information
    conn = sqlite3.connect(os.path.join('./resources', 'flog.db'))
    c = conn.cursor()
    c.execute('''create table airports(
            airport_id integer primary key,
            icao text not null,
            iata text,
            name text,
            lat real,
            long real,
            country text,
            alt integer,
            utcoffset integer,
            tzolson text)''')
    conn.close()


def deletedbairports(): # deletes flights database. Use with caution!
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute("DROP TABLE airports")
    print('Table airports has been deleted')
    conn.close()


def createdbpilots(): # this database will hold Pilot names
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute('''create table pilots(
            pilot_id integer primary key,
            picfirstname text,
            piclastname text not null,
            alias text)''')
    firstname = input('Please enter your First name: ')
    lastname = input('Please enter your Last name: ')
    todb = (firstname,lastname,'self')
    c.execute("INSERT INTO pilots (\
                                      picfirstname,\
                                      piclastname,\
                                      alias) \
                                      VALUES (?,?,?)", todb)
    conn.commit()
    conn.close()


def deletedbpilots(): # deletes flights database. Use with caution!
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute("DROP TABLE pilots")
    print('Table pilots has been deleted')
    conn.close()


def createdbaircraft():
    """
    This database will hold AC information. Some triggers
    like multiengine or heavy (above 5.7t) should be 
    Boolean but sqlite does not have a seperate type
    definition so 1=True, 0=False. Seperate table needs
    to be created for Registrations (Tails)
    """
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute('''create table aircraft(
            aircraft_id integer primary key,
            Name text,
            iata text,
            icao text,
            multipilot integer,
            multiengine integer,
            jet integer,
            heavy integer)''') #heavy as in above 5.7t
    conn.close()


def deletedbaircraft(): # deletes aircraft database. Use with caution!
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute("DROP TABLE aircraft")
    print('Table aircraft has been deleted')
    conn.close()


def createdbtails():
    """
    This table links individual aircraft to an aircraft
    type specified in the aircraft table.
    """
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute('''create table tails(
            tail_id integer primary key,
            Registration text not null,
            aircraft_id integer not null,
            Company Text,
            foreign key (aircraft_id) references aircraft (aircraft_id)
            )''') #heavy as in above 5.7t
    conn.close()


def deletedbtails(): # deletes extras database. Use with caution!
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute("DROP TABLE tails")
    print('Table tails has been deleted')
    conn.close()


def createdbextras():
    """
    This database will hold additional information like function
    times, operational condition time, night flying
    time...
    """
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute('''create table extras(
            extras_id integer primary key,
            PilotFlying integer,
            TOday integer,
            TOnight integer,
            LDGday integer,
            LDGnight integer,
            autoland integer,
            tNight integer,
            tIFR integer,
            tPIC integer,
            tSIC integer,
            tDual integer,
            tInstructor integer,
            tSIM integer,
            ApproachType text,
            FlightNumber text,
            Remarks text)''')
    conn.close()


def deletedbextras(): # deletes extras database. Use with caution!
    conn = sqlite3.connect(os.path.join('./csv', 'flog.db'))
    c = conn.cursor()
    c.execute("DROP TABLE extras")
    print('Table extras has been deleted')
    conn.close()
