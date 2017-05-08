### image_data_viewer

This is an example that uses OpenGL to enable a lightweight application for viewing video data.

It receives `ps_image_data_msg` from a node publishing to the bus, and renders the image data contained in the message using OpenGL libraries.

Note that it binds to the first image data publisher it finds, and that it is expecting `PIXEL_FORMAT_RGB24`. 

### Hardware requirements

Video device

### Dependencies

Packages: libglib2.0-dev freeglut3-dev

To install on Ubuntu: 

```bash
sudo apt-get install libglib2.0-dev freeglut3-dev
```

### Building and running the node

```bash
$ cd image_data_viewer
$ make
$ ./bin/polysync-image-data-viewer 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
