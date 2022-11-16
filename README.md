Introduction
------------

Simple QWebEngine wrapper for MaterialSkin on LMS. This application requires
MaterialSkin to be installed on LMS but does not need to be set as the default
skin.

Building
--------
1. sudo apt install cmake g++ qtbase5-dev qtwebengine5-dev
2. mkdir build
3. cd build
4. cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release
5. make
6. sudo make install
