### user_data_model

This is a use case example for modifying and regenerating the PolySync data model using the provided `pdm-gen` tool.

For more information see: [Extending A Data Model Tutorial](http://docs.polysync.io/articles/tutorials-and-examples/tutorials/extending-a-data-model-tutorial/).

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install libglib2.0-dev
```

### Building and running the node

```bash
$ cd user_data_model
$ make
$ ./bin/polysync-user-data-model-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
