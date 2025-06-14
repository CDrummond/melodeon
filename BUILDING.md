Cloning
-------
1. git clone http://github.com/CDrummond/melodeon.git
2. cd  melodeon
3. git submodule update --init --recursive

Build
-----
1. cd melodeon
2. sudo apt install cmake g++ qt6-base-dev qt6-svg-dev qt6-webengine-dev
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

Create Snap package
-------------------
These steps assume you do not have Docker installed as it may conflict with lxd which is used by snapcraft.
Also, enure you have plenty of spare disk space, the build process can easily consume 10-20G under /var.
1. Perform cloning steps 1-3.
2. `snap install snapcraft --classic`
3. `snap install --channel 5.21/stable lxd`
4. `sudo usermod -a -G lxd $USER`
5. Logout and back in to pickup the new group membership.
6. `lxd init --minimal`
7. `snapcraft pack`
8. A .snap file will be produced in the current directory.
9. Install with: `snap install ./<name>.snap --dangerous`
10. The `--dangerous` argument is required as the snap is not signed by the snapstore.
