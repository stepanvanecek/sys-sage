# sys-sage

sys-sage is a library for capturing and manipulating hardware topology of compute systems.

It is currently under development, so there may be breaking changes in the API in the future.

### Documentation

The documentation is available [here](https://stepanvanecek.github.io/sys-sage/html/index.html).

### Installation

Please refer to the [Installation Guide](https://stepanvanecek.github.io/sys-sage/html/md__installation__guide.html) for more information.

The recommended installation is through spack, alternatively, sys-sage can be built from the sources.

```bash
spack install sys-sage
#see "spack info sys-sage" for available options and versions
```

Installation with CMAKE:
```bash
mkdir build && cd build
cmake ..
# build options:
# -DCAT_AWARE=ON            - builds with Intel CAT functionality. For that, Intel-specific pqos header/library are necessary.
# -DDATA_SOURCES=ON         - builds all data sources from folder 'data-sources' listed below. Data sources are used to collecting HW-related information, so it only makes sense to compile that on the system where the topology information is queried.
# -DDS_HWLOC=ON             - builds the hwloc data source for retrieving the CPU topology
# -DDS_MT4g=ON              - builds the mt4g data source for retrieving GPU compute and memory topology. If turned on, includes hwloc.
# -DDS_NUMA=ON              - builds the caps-numa-benchmark. If turned on, includes Linux-specific libraries.

# -DCMAKE_INSTALL_PREFIX=../inst-dir    - to install locally into the git repo folder
make all install
```

### About

sys-sage has been created by Stepan Vanecek (stepan.vanecek@tum.de) and the [CAPS TUM](https://www.ce.cit.tum.de/en/caps/homepage/). Please contact us in case of questions, bug reporting etc.

sys-sage is available under the LGPL-2.1 license. (see [License](https://github.com/caps-tum/sys-sage/blob/master/LICENSE))
