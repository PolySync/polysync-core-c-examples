### parrot_controller
This is a simple example to demonstrate how one would send control commands to a vehicle in PolySync
Receives platform motion messages which indicate parrot position, and calculates `ps_platform_steering_command_msg`, and `ps_platform_throttle_command_msg` commands to get the parrot to waypoints designated on screen. 

For more detailed information look at the [Vehicle Control Tutorial](https://help.polysync.io/articles/tutorials-and-examples/tutorials/vehicle-control-tutorial/)

### Hardware requirements

### Dependencies

### Building and Running the Node
```bash
$ cd parrot_controller
$ make
$ ./polysync-parrot-controller 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
