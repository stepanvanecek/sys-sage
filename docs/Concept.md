# Concept

The main goal of the library is to to store, update, and provide all relevant information about **hardware topology, dynamic system state/configuration, system capabilities, and other data related to HW from different data sources logically connected to each other**.

## Defining the functionality

#### sys-sage vs. hwloc
sys-sage can be considered an extension to hwloc; it targets the aspects of system topology and HW-related information that are not covered by hwloc.
Hwloc is limited to (among otehrs)
- Providing static data (only given HW topology)
    - Modern systems are not strictly hierarchical anymore
    - Information regarding data movement capabilities is missing
- Difficult to incorporate complementary information
- Mainly CPU-centric

#### What sys-sage Addresses
On the top of hwloc functionality, sys-sage targets
- **The dynamic aspects of modern HPC systems**
    - Data movement information
    - Variable system characteristics
- **Managing arbitrary systems including state-of-the-art architectures**
    - heterogeneous components (CPU, GPU, ..)
    - Interconnets / buses connecting the components
- **High variability in storing and using the data**
    - Different set of information needed ind ifferent use-cases
    - sys-sage enables representing an entity from a single chip up to a whole HPC system with custom level of detail )
    - Arbitrary data can be added to already existing representation
    - sys-sage can store/maintain/provide arbitrary information out-of-the-box, i.e. the user is free to define the stored parameters

#### Possible Areas of Usage

sys-sage is designed to be very versatile, so that it can be integrated to work in many different areas. Thanks the variability, the users can use the fitting set of information for various use-cases.

Some of the areas may include:
- Job / thread scheduling, co-scheduling multiple applications
- Autotuning tasks / applications
- Data management on heterogeneous memory systems (allocation decisions)
- Power management
- Performance optimization, performance modelling tools
- ...and many others

#### Functionality scope

The library aims at storing, and providing the data to the user, not necessarily at the data collection.
Nevertheless, it also provides a set of so-called Default Data Sources which provide the often-used set of information out-of-the-box.
It can represent the dynamic state of the system and increases the degree of detail compared to static hardware topology views.

![alt text](images/goal.drawio.pdf)

#### Types of Attributes to Handle
1. Both qualitative (such as core ID) and quantitative (cache size) attributes.
2. Information describing one particular component (L1 cache), their group
(socket), or relation between them (CPU-GPU bandwidth).
3. Both static (theoretical peak bandwidth) and dynamic variable (currently
measured bandwidth) information.
4. Data to describe the hardware (CPU cores), the configuration of a system
(hyperthreading enabled), or its current state (wattage on CPU).
5. Both system-centric (containing information about all system resources) and application-centric (containing information about the resources available to
or used by a particular application/process/user).
6. Application-relevant observations (current memory occupancy, monitoring
data, ...).

## Design of sys-sage
