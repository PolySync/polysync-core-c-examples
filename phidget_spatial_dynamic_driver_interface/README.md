### phidget_spatial_dynamic_driver_interface
This is an example of the dynamic driver interface, the official interface for any hardware device or sensor data to get into a PolySync system.
This is an inexpensive accelerometer sensor that’s easy to access to be able to tweak and learn more about the dynamic driver interface code.
Defines a Hardware Abstraction Layer (HAL)
Defines a PolySync Abstraction Layer (PAL)
For more information see [Creating A Dynamic Driver Interface Workflow](https://help.polysync.io/articles/development/dynamic-driver-hw-interface-development/creating-a-dynamic-driver-interface/)

### Hardware requirements

### Dependencies

### Building and Running the Node
* To build the full project, first build the HAL (drivers) then the PAL (interfaces)

1. Build the Hardware Abstraction Layer (HAL)

```bash
$ cd drivers/phidet_spatial
$ make
```

2. Build the PolySync Abstraction Layer (PAL)

```bash
$ cd interfaces/phidget_spatial
$ make
$ make install   # installs to PSYNC_HOME/lib/ 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
