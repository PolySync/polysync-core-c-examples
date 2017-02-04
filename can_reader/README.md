
### CAN reader
Using the PolySync state machine this example does a simple CAN read in OK the state.
This example will give you experience for the times you need a node to listen to a CAN channel.

### Hardware requirements
- linuxcan-compatible hardware is connected, i.e. a Kvaser Leaf Light

### Dependencies
`sudo apt-get install libgtk2.0-dev`


/usr/bin/ld: cannot find -ldcpssac
/usr/bin/ld: cannot find -ldcpsgapi
/usr/bin/ld: cannot find -lddsuser
/usr/bin/ld: cannot find -lddskernel
/usr/bin/ld: cannot find -lddsserialization
/usr/bin/ld: cannot find -lddsconfparser
/usr/bin/ld: cannot find -lddsconf
/usr/bin/ld: cannot find -lddsdatabase
/usr/bin/ld: cannot find -lddsutil
/usr/bin/ld: cannot find -lddsos
/usr/bin/ld: cannot find -lcurl
/usr/bin/ld: cannot find -lsqlite3
/usr/bin/ld: cannot find -lxml2


### Building and Running the Node
```bash
$ cd can_reader
$ make
# You need to have an CAN channel to run this example see:  [Connecting To A CAN Sensor](https://help.polysync.io/articles/configuration/runtime-node-configuration/connecting-to-a-can-radar-sensor/)
$ ./bin/polysync-can-reader-c 1
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)