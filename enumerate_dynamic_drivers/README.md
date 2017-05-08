### Enumerate dynamic drivers

This example publishes a `ps_command_msg` to enumerate any dynamic drivers connected to the
PolySync bus.  All dynamic drivers that have a GUID and valid node ID from the SDF Configurator will receive all `ps_response_msg`s that are published to the bus.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install libglib2.0-dev
```

### Building and running the node

```bash
$ cd enumerate_dynamic_drivers
$ make
$ ./bin/polysync-enumerate-dynamic-drivers
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
