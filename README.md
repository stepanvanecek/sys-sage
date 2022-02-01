# sys-sage

Library for capturing and manipulating hadrware topology of compute systems.

Still in development, no stable release available yet.



mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../inst-dir ..
make all install

add_definitions(-DCAT_AWARE )
-DBUILD_DATA_SOURCES=ON
