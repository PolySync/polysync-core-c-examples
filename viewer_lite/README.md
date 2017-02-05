### viewer_lite
This example is more of a tool than an API example.
It will visualize LiDAR, RADAR and object data from multiple sources.
This example has tools to freeze-frame and measure distance; color by source or type; great for building sensor fusion applications.
The graphics were built with OpenGL and are very extendable.
It has a built in system/node management tool to dynamically handle nodes coming on/off bus.

### Hardware requirements

### Dependencies
Packages: libglib2.0-dev freeglut3-dev
To install on Ubuntu
`sudo apt-get install <package>`

### Building and Running the Node
```bash
$ cd viewer_lite
$ make
$ ./bin/polysync-viewer-lite 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
