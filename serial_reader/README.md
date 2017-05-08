### serial_reader

You would use this as a reference if you needed a node to listen to a device over a serial connection.

This example creates and reads data through a serial connection, to a hardware device, or another software application.
It exercises the Serial API.

You can use this with the `serial_writer` example.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install libglib2.0-dev
```

### Building and running the node

```bash
$ cd serial_reader
$ make
$ ./bin/polysync-serial-reader-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
