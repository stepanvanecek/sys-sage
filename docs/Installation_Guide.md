# Installation Guide

The recommended installation is through spack, alternatively, sys-sage can be built from the sources.

## spack installation

Using [spack](https://github.com/spack/spack) (see [spack documentation](https://spack.readthedocs.io/en/latest/))

```bash
spack install sys-sage
```

There are the following options (also see `spack info sys-sage`):
- **intel_cat** *default=False* -- Build with Intel CAT(Cache Allocation Technology) to obtain cache partitioning information (Intel only, skylake and later).
- **build_data_sources** *default=False* -- Build data sources (programs to collect data about the machine sys-sage runs on). Uses Linux-specific libraries, so Linux-only.

## From the sources

sys-sage uses CMake as a build system.

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
