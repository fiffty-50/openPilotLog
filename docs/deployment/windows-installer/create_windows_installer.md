# Creating a windows installer:

## 1 Update Version Info

Update the version numbers in
- `\packages\com.opl.openPilotLog\meta\package.xml`
- `\config\config.xml`

## 2 Collect dependencies

- Use windeployqt to bundle the dependencies into a deployment directory

`.\windeployqt.exe --dir \path\to\deploy-dir\ \path\to\openPilotLog.exe`

## 3 Add openSSL binaries

- These libraries are required for connectivity and cannot be bundled by windeployqt, so copy them into the deployment directory

`libcrypto-1_1-x64.dll`

`libssl-1_1-x64.dll`

## 4 Zip the deployment directory

- create a 7zip archive of the deployment directory and put it in `\packages\com.opl.openPilotLog\data`

## 5 create installer with Qt Installer Framework

- Use the Qt Installer Framework binary creator to create an installer

`.\binarycreator.exe -c \path\to\config.xml -p \path\to\directory\packages \path\to\output_file.exe`