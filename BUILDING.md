Build
-----
1. sudo apt install cmake g++ qtbase5-dev qtwebengine5-dev
2. mkdir build
3. cd build
4. cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release
5. make
6. sudo make install


Create DEB package
------------------
1. sudo apt install dpkg-dev
2. Perform steps 1..5 of `Building` (above)
3. cpack -G DEB
