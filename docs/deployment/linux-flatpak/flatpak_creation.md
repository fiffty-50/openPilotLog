# To Do

Create a repository to host the flatpak and upload to flathub

# For now - How to create a local flatpak

## Requirements

Install flatpak and the required Platform and make sure they match the versions in the `yaml` file. Details on the [docs](https://docs.flatpak.org/en/latest/first-build.html)

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

flatpak install flathub org.kde.Sdk
flatpak install flathub org.kde.Platform
```

## Build the package

Build the package and add it to a repository - `repo` in this case. Add the repository.

```bash
# flatpak-builder --repo=repo --user --install --farce-clean [build directory] [manifest file]

flatpak-builder --repo=repo --user --install --force-clean ~/git/flatpak-builddir ~/Documents/org.opl.openPilotLog.yaml 

flatpak --user remote-add --no-gpg-verify repo repo
```
Build an [application bundle](https://docs.flatpak.org/en/latest/single-file-bundles.html) from the local repository
```bash
# flatpak build-bundle [repository] [bundle name] [application name]
flatpak build-bundle repo openPilotLog.flatpak org.opl.openPilotLog
```
## Install the flatpak package locally

On the target machine, navigate to the download directory and install the .flatpak file

```bash
flatpak install --user openPilotLog.flatpak
````