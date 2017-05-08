### parrot_controller

This is an example that demonstrates how to send high level control commands to a vehicle in PolySync.

This example receives platform motion messages, which indicate the parrot's position, and calculates `ps_platform_steering_command_msg` and `ps_platform_throttle_command_msg` commands to get the parrot to waypoints designated on screen from the `parrot_visualizer` example. 

For more detailed information look at the [Vehicle Control Tutorial](https://help.polysync.io/articles/tutorials-and-examples/tutorials/vehicle-control-tutorial/).

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install libglib2.0-dev
```

### Building and running the node

```bash
$ cd parrot_controller
$ make
$ ./polysync-parrot-controller 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
