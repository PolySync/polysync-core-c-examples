### node_template

This example shows the basic “node template” that all PolySync nodes use.

The different node states include:
   - set_configuration: Is called first of the node state callbacks. Typically operates on command line arguments the user has passed, if any, and does initial node configuration
   - on_init: called when a PS node translates into the INIT state; which is after set_configuration
   - on_release: is called once on node exit. Should deallocate any memory, close any connections, etc
   - on_error: is called continuously when the node is in an error state. should handle the error and respond appropriately
   - on_fatal: called once when the node translates into a fatal state, before exiting.
   - on_warn: called continuously when the system is in the warn state.
   - on_ok: called continuously when the system is in an ok state. This is where the main code of the node should go; to be executed when everything is operating properly.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and Running the Node

```bash
$ cd node_template
$ make
$ ./bin/polysync-node-template-c 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
