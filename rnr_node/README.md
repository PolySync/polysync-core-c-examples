### rnr_node

This is an example of both the Record and Replay API, and the coupled Logfile API.

It is an example of a node that listens and responds to the `ps_rnr_msg` messages, which command the node to use Logfile API resources that facilitate synchronized record and replay across all nodes on the system.

You would use this example if you needed a node to log incoming messages (that it’s subscribed to), to be able to replay later. 

PolySync provides the dynamic drivers that record sensor data, but you may also want to record the algorithm inputs and/or outputs.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and running the node

```bash
$ cd rnr_node
$ make
$ ./bin/polysync-rnr-node-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
