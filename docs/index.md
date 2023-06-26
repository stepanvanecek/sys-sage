# sys-sage Documentation

This is the documentation of sys-sage.

sys-sage is a software library with a C++ API.
The main goal of the library is to to store, update, and provide all relevant information about **hardware topology, dynamic system state/configuration, system capabilities, and other data related to HW from different data sources logically connected to each other**.

### Resources

- [sys-sage Library Concept](Concept.md)
- [Installation Guide](Installation_Guide.md)
- **API documentation**
    - [**Component**](class_component.html) ( [Topology](class_topology.html), [Node](class_node.html), [Memory](class_memory.html), [Storage](class_storage.html),  [Chip](class_chip.html), [Cache](class_cache.html), [Subdivision](class_subdivision.html), [Numa](class_numa.html), [Core](class_core.html), [Thread](class_thread.html) )
    - [**Data Path**](class_data_path.html)
    - Input Parsers
        - [**hwloc parser**](hwloc_8hpp.html)
    - Data Sources
        - [**hwloc**](hwloc-output_8cpp.html)

### About

sys-sage has been created by Stepan Vanecek (stepan.vanecek@tum.de) and the [CAPS TUM](https://www.ce.cit.tum.de/en/caps/homepage/). Please contact us in case of questions, bug reporting etc.

The source code can be found at [https://github.com/caps-tum/sys-sage](https://github.com/caps-tum/sys-sage) .

sys-sage is available under the LGPL-2.1 license. (see [License](https://github.com/caps-tum/sys-sage/blob/master/LICENSE))
