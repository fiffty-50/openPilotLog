#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 28 11:23:44 2020
Used to import csv data into tables, written in python for ease of use.
Data is obtained from:
https://ourairports.com/data/
https://openflights.org/data.html
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
import csv
import sqlite3
import os

conn = sqlite3.connect(os.path.join('./csv', 'logbook.db'))
c = conn.cursor()

with open(os.path.join('./csv', 'airports_edited.csv')) as fin:
    dr = csv.DictReader(fin)  # comma is default delimiter
    to_dbap = [(i['icao'],
              i['iata'],
              i['name'], 
              i['lat'],
              i['long'],
              i['country'],
              i['alt'],
              i['utcoffset'],
              i['tzolson'],
              ) for i in dr]


def csvtodbap(to_db):
    c.executemany("INSERT INTO airports (icao, iata, name, lat, long, country, alt, utcoffset, tzolson) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);", to_db)
    conn.commit()
    print('Database entries have been added. New table:')
    for row in c.execute("select * from airports"):
        print(row)
    conn.close()


with open(os.path.join('./csv', 'flights_test.csv')) as fin:  # imports flights
    dr = csv.DictReader(fin)  # comma is default delimiter
    to_dbfl = [(i['doft'],
              i['dept'],
              i['tofb'],
              i['dest'],
              i['tonb'],
              i['tblk'],
              i['pic'],
              i['acft']
              ) for i in dr]


def csvtodbfl(to_db):
    c.executemany("INSERT INTO flights (doft, dept, tofb, dest, tonb, tblk, pic, acft) VALUES (?, ?, ?, ?, ?, ?, ?, ?);", to_db)
    conn.commit()
    print('Databse entries have been added. New table:')
    for row in c.execute("select * from flights"):
        print(row)
    conn.close()


with open(os.path.join('./csv', 'pilots_test.csv')) as fin:  # imports flights
    dr = csv.DictReader(fin)  # comma is default delimiter
    to_dbpl = [(i['picfirstname'],
              i['piclastname'],
              i['alias']
              ) for i in dr]


def csvtodbpl(to_db):
    c.executemany("INSERT INTO pilots (picfirstname, piclastname, alias) VALUES (?, ?, ?);", to_db)
    conn.commit()
    print('Database entries have been added. New table:')
    for row in c.execute("select * from pilots"):
        print(row)
    conn.close()


with open(os.path.join('./csv', 'aircraft.csv')) as fin:  # imports flights
    dr = csv.DictReader(fin)  # comma is default delimiter
    to_dbac = [(i['Name'],
              i['iata'],
              i['icao'],
              ) for i in dr]


def csvtodbac(to_db):
    
    c.executemany("INSERT INTO aircraft (aircraft_id,make,model,variant,name,iata,icao,singlepilot,multipilot,singleengine,multiengine,turboprop,jet,heavy) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", to_db)
    conn.commit()
    print('Databse entries have been added. New table:')
    for row in c.execute("select * from aircraft"):
        print(row)
    conn.close()


with open(os.path.join('./csv', 'tails_test.csv')) as fin: # imports flights
    dr = csv.DictReader(fin) # comma is default delimiter
    to_dbtails = [(i['Registration'], 
              i['aircraft_id'],
              i['Company']
              ) for i in dr]


def csvtodbtails(to_db):
    c.executemany("INSERT INTO tails (Registration, aircraft_id, Company) VALUES (?, ?, ?);", to_db)
    conn.commit()
    print('Databse entries have been added. New table:')
    for row in c.execute("select * from tails"):
        print(row)
    conn.close()
