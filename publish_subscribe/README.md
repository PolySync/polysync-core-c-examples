### publish_subscribe

This example shows simple publish and subscribe functionality in PolySync, the most common form of node-to-node communication in a PolySync system.

It publishes an empty `ps_event_msg` to show how the publish functionality works in PolySync.

It also subscribes to all diagnostic messages on the PolySync bus, and then prints the info each diagnostic message contains as they are received in real time.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and running the node

```bash
$ cd publish_subscribe
$ make
$ ./bin/polysync-publish-subscribe-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
