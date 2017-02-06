### logfile_iterator_for_velodyne
This example is the same as the logfile_iterator example, except that it provides a concrete example of iterating over LiDAR logfile data stored as a `ps_byte_array_msg`
Specifically shows how to use the dynamic driver Hardware Abstraction Layer (HAL) header file for the Velodyne HDL to cast raw, non-abstracted `plog` data to the low-level OEM defined C structs

### Dependencies
Packages: libglib2.0-dev
To install on Ubuntu
`sudo apt-get install <package>`

### Building and Running the Node
```bash
$ cd logfile_iterator_for_velodyne
$ make
$ ./bin/polysync-logfile-iterator-for-velodyne-c 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
