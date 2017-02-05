### image_data_viewer
This example shows you how to use the get/set API to modify node parameters.  It uses the `ps_parameters_msg`
to get all parameters from all nodes on the bus.  It also receives a response `ps_parameters_msg` in a callback and
prints the resulting parameters.

### Hardware requirements

### Dependencies
Install Dependencies
`$ sudo apt-get install libgtk2.0-dev`
`$ sudo apt-get install freeglut3-dev`

### Building and Running the Node
```bash
$ cd image_data_viewer
$ make
$ ./bin/polysync-image-data-viewer 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
