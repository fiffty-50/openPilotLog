# Contents

- [General UI Design](#general)
- [Description of Widgets](#Widgets)
    - [HomeWidget](#HomeWidget)
    - [LogbookWidget](#LogbookWidget)
    - [PilotsWidget](#PilotsWidget)
    - [SettingsWidget](#SettingsWidget)
    - [SettingsWidget](#SettingsWidget)
- [Description of the Dialogs](#Dialogs)
    - [NewPilotDialog](#NewPilotDialog)
    - [NewTailDialog](#NewTailDialog)
    - [NewFlightDialog](#NewFlightDialog)

## General UI Design <a name="general"></a>

<details><summary>Screenshot</summary>
<p>

![example](https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/screenshots/default_view_system_theme.png)

<p>
</details>

The general design of this application shall align with the following guiding principles:

- KISS - Keep it simple stupid
    - The purpose of this application is to act as a logbook, it's not rocket science and it should not pretend to be. Logging a flight should be as simple and straightforward as possible.
    - That being said, it is important that what is done, is done right. Don't take shortcuts when it comes to core functionality.
    - The user interface shall be lightweight and intuitive
    - Do not get in the users way. Restrict where necessary but allow freedom of choice where possible.


The purpose of this application is to be lightweight, reliable, customizable and stay out of the users way.

## PilotsWidget <a name="PilotsWidget"></a>

In the PilotsWiget, a list of the pilots the user has added to the database is shown. On the left side of the screen, all pilots are shown in a table, with their last name, first name and company. On the right side, a line edit is available where the table view can be filtered and searched.

Once the user selects an entry in the table view, a [NewPilotDialog](#NewPilotDialog) is shown as a widget on the right side of the screen. Here, the user can see all the details related to the entry and edit it.

Below the table view are push buttons that enable the user to create new entries or edit existing ones.

## New Pilot Dialog <a name="NewPilotDialog"></a>

The New Pilot Dialog is used to add new pilots to the database or edit existing ones. It has 7 line edits, which represent the columns of the pilots table in the database. 
