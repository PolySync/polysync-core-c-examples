### image_data_viewer
This is an example that uses opengl to enable a lightweight application for viewing video data.
It receives `ps_image_data_msg` from a node publishing to the bus.
It renders the image data contained in `ps_image_data_msg` using opengl libraries.
It binds to the first image data publisher it finds.
It Expects XXXXX pixel format.

### Hardware requirements
Video device

### Dependencies
Packages: libglib2.0-dev freeglut3-dev
To install on Ubuntu
`sudo apt-get install <package>`

### Building and Running the Node
```bash
$ cd image_data_viewer
$ make
$ ./bin/polysync-image-data-viewer 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
