### data_generator

This example covers how to populate and publish data from a single node into a PolySync message on the bus, 
enable multiple nodes to subscribe to messages to access data asynchronously, and visualize all data being 
published to the PolySync bus.

This example generates data and publishes it on the PolySync bus by generating RADAR, LiDAR and object data.

It does this by periodically publishing `ps_lidar_points_msg`, `ps_radar_targets_msg` and `ps_objects_msg` to the bus in OK state.

There is Tutorial in the Help Center that describes the C++ version of this example: [Data Generation Tutorial](https://help.polysync.io/articles/tutorials-and-examples/tutorials/data-generation-tutorial/).

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and Running the Node

```bash
$ cd data_generator
$ make
$ ./bin/polysync-data-generator-c
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)