### logfile_iterator_for_velodyne

This example is based on the logfile_iterator example. It differs in that it provides a concrete example of iterating over Velodyne LiDAR logfile data stored as a `ps_byte_array_msg` in a `plog` file.

Within the logfile iterator callback, this example shows how to use the dynamic driver Hardware Abstraction Layer (HAL) header file for the Velodyne HDL to cast raw, non-abstracted `plog` data to the low-level OEM defined C structs.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and running the node

```bash
$ cd logfile_iterator_for_velodyne
$ make
$ ./bin/polysync-logfile-iterator-for-velodyne-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
