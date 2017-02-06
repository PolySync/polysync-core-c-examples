### node_template

This example shows the basic “node template” that all PolySync nodes use.

The different node states include:
   - set_configuration: The first called out of the node state callbacks. Typically operates on command line arguments the user has passed, if any, and performs initial node configuration.
   - on_init: This is called when a PS node translates into the INIT state, which is after set_configuration.
   - on_release: This is called once on node exit. Should deallocate any memory, close any connections, etc.
   - on_error: This is called continuously when the node is in an error state. Should handle the error and respond appropriately.
   - on_fatal: This is called once when the node translates into a fatal state, before exiting.
   - on_warn: This is called continuously when the system is in the warn state.
   - on_ok: This called continuously when the system is in an OK state. This is where the main code of the node should go; to be executed when everything is operating properly.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and running the node

```bash
$ cd node_template
$ make
$ ./bin/polysync-node-template-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/),
