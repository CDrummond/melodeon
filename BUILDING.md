Cloning
-------
1. git clone http://github.com/CDrummond/melodeon.git
2. cd  melodeon
3. git submodule update --init --recursive

Build
-----
1. cd melodeon
2. sudo apt install cmake g++ qt6-base-dev qt6-webengine-dev
3. mkdir build
4. cd build
5. cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release
6. make
7. sudo make install


Create DEB package
------------------
1. sudo apt install dpkg-dev
2. Perform steps 1..6 of `Building` (above)
3. cpack -G DEB
