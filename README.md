# sys-sage

Library for capturing and manipulating hadrware topology of compute systems.

Still in development, no stable release available yet.

Installation with CMAKE:
```bash
mkdir build && cd build
cmake ..
# build options:
# -DCAT_AWARE=ON            - builds with Intel CAT functionality. For that, Intel-specific pqos header/library are necessary.
# -DBUILD_DATA_SOURCES=ON   - builds data sources from folder 'data-sources'. If turned on, includes Linux-specific libraries and hwloc. Data sources are used to collecting HW-related information, so it only makes sense to compile that on the system where the topology information is queried.
# -DCMAKE_INSTALL_PREFIX=../inst-dir    - to install locally into the git repo folder
make all install
```
