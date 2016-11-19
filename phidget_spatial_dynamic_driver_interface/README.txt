======================================================
Readme
======================================================

* This is a working directory for creating the Phidget Spatial Dynamic Driver interface

* For a tutorial on creating a custom Dynamic Driver interface, see this article in the Help Center: 
  * https://help.polysync.io/prerelease/?article=creating-a-dynamic-driver-interface

* The Hardware Abstraction Layer (HAL) include files are available for applications to access OEM data structures rather than high-level PolySync messages
  * Here's a C API example that exercises the Velodyne HDL HAL: https://github.com/PolySync/PolySync-C-Examples/tree/master/logfile_iterator_for_velodyne

polysync/dynamic_driver/
├── drivers
│   └── phidget_spatial
│       ├── include
│       │   └── phidget_spatial_driver.h
│       ├── Makefile
│       └── src
│           ├── phidget_spatial_driver.c
│           └── phidget_spatial_test_interface.c
├── include
│   ├── phidget_spatial_driver.h
│   ├── polysync_dynamic_driver_commands.h
│   ├── polysync_dynamic_driver.h
│   ├── polysync_dynamic_driver_parameters.h
│   ├── polysync_dynamic_interface.h
│   └── polysync_interface_util.h
├── interfaces
│   ├── example
│   │   ├── Makefile
│   │   └── src
│   │       └── interface.c
│   └── phidget_spatial
│       ├── include
│       │   ├── connect.h
│       │   ├── debug.h
│       │   ├── diagnostics.h
│       │   ├── getset.h
│       │   ├── interface_data.h
│       │   ├── log.h
│       │   ├── options.h
│       │   ├── process.h
│       │   ├── read.h
│       │   ├── sdf.h
│       │   └── type_support.h
│       ├── Makefile
│       └── src
│           ├── connect.c
│           ├── debug.c
│           ├── diagnostics.c
│           ├── getset.c
│           ├── interface.c
│           ├── log.c
│           ├── options.c
│           ├── process.c
│           ├── read.c
│           ├── sdf.c
│           └── type_support.c
├── lib
└── README.txt


======================================================
Building
======================================================

* To build the full project, first build the HAL (drivers) then the PAL (interfaces)

1. Build the Hardware Abstraction Layer (HAL)

$ cd drivers/phidet_spatial
$ make

2. Build the PolySync Abstraction Layer (PAL)

$ cd interfaces/phidget_spatial
$ make
$ make install   # installs to PSYNC_HOME/lib/ 

