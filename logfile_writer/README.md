### logfile_writer

This is an example that uses the Logfile API to replay a PolySync logfile (`plog` file).

It shows how to use the Logfile API routines to write a PolySync byte array message to a `plog` file.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install libglib2.0-dev
```

### Building and running the node

```bash
$ cd logfile_writer
$ make
$ ./bin/polysync-logfile-writer-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
