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

