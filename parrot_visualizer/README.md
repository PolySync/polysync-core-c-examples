### parrot_visualizer

This is a simple example that “simulates” how a vehicle would respond to actuation commands in PolySync.

It receives both `ps_platform_steering_command_msg` and `ps_platform_throttle_command_msg`, and actuates the parrot as specified.

It publishes back platform motion messages to indicate the parrot's position. It depends on the `parrot_controller` example running.

For more detailed information, look at the [Vehicle Control Tutorial](https://help.polysync.io/articles/tutorials-and-examples/tutorials/vehicle-control-tutorial/).

### Dependencies

Packages: libglib2.0-dev freeglut3-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and running the node

```bash
$ cd parrot_visualizer
$ make
$ ./polysync-parrot-visualizer
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
