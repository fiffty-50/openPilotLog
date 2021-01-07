https://docs.appimage.org/packaging-guide/manual.html#ref-manual

https://docs.appimage.org/packaging-guide/overview.html#packaging-from-source


## Set up environment

sudo apt install -y python3-pip python3-setuptools patchelf desktop-file-utils libgdk-pixbuf2.0-dev fakeroot strace

sudo wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage -O /usr/local/bin/appimagetool
sudo chmod +x /usr/local/bin/appimagetool

sudo pip3 install appimage-builder

from Appdir ../ 
run appimage-builder --generate

appimage-builder --skip-test --skip-appimage

appimagetool AppDir/