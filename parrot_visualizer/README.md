### parrot_visualizer
This is a simple example to “simulate” how a vehicle would respond to actuation commands in PolySync.
Receives `ps_platform_steering_command_msg`, `ps_platform_throttle_command_msg`, and actuates the parrot as specified.
It publishes back platform motion messages to indicate parrot position.

For more detailed information look at the [Vehicle Control Tutorial](https://help.polysync.io/articles/tutorials-and-examples/tutorials/vehicle-control-tutorial/)

### Dependencies
Packages: libglib2.0-dev freeglut3-dev
To install on Ubuntu
`sudo apt-get install <package>`

### Building and Running the Node
```bash
$ cd parrot_visualizer
$ make
$ ./polysync-parrot-visualizer
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
