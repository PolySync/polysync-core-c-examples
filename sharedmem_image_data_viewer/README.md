### sharedmem_image_data_viewer

This example is the same as the `image_data_viewer` example, but uses the shared memory queue rather than the Ethernet bus to transmit the incoming image sensor data.

You must set up the shared memory queue buffer size prior to starting the application.

Note that it expects RGB24 pixel format.

### Hardware requirements

Video device

### Dependencies

Packages: libglib2.0-dev freeglut3-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and Running the Node

```bash
$ cd sharedmem_image_data_viewer
$ make
$ ./bin/polysync-sharedmem-image-data-viewer 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
