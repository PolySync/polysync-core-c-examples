### joystick_commander
This example shows you how to use a joystick to control a vehicle.  
If you want to start controlling a vehicle this is the best place to start.  
This example periodically sends `ps_platform_brake_command_msg`,  `ps_platform_steering_command_msg`, and 
`ps_platform_throttle_command_msg` out on the PS bus.
The actuation commands are calculated from the joystick position.  
It also can do high level management of control states; “enables” and "disables" vehicle control.

### Dependencies
Packages: libglib2.0-dev libsdl2-dev
To install on Ubuntu
`sudo apt-get install <package>`

### Building and Running the Node
```bash
$ cd joystick_commander
$ make
$ ./bin/polysync-joystick-commander 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
